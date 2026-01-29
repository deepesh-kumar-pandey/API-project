#include "../include/Rate_limiter.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>

using namespace std;


// Get encryption key from environment variable (more secure than hardcoding)
static std::string get_encryption_key() {
    const char* env_key = std::getenv("GATEKEEPER_KEY");
    if (env_key && strlen(env_key) > 0) {
        return std::string(env_key);
    }
    // In production, we should fail fast if the key is missing
    std::cerr << "[CRITICAL ERROR] GATEKEEPER_KEY is not set. Data encryption will fail.\n";
    std::cerr << "[ADVICE] Set GATEKEEPER_KEY as an environment variable before running.\n";
    exit(1); 
}


static const std::string ENCRYPTION_KEY = get_encryption_key();

static std::vector<unsigned char> derive_aes_key(const std::string& secret) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)secret.c_str(), secret.length(), hash);
    return std::vector<unsigned char>(hash, hash + SHA256_DIGEST_LENGTH);
}

static std::string aes_256_encrypt(const std::string& plaintext, const std::string& secret) {
    auto key = derive_aes_key(secret);
    unsigned char iv[16];
    if (RAND_bytes(iv, sizeof(iv)) != 1) return "";

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv);

    std::vector<unsigned char> ciphertext(plaintext.length() + EVP_MAX_BLOCK_LENGTH);
    int len, ciphertext_len;
    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, (const unsigned char*)plaintext.c_str(), plaintext.length());
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);

    std::string result((char*)iv, 16);
    result.append((char*)ciphertext.data(), ciphertext_len);
    return result;
}

static std::string aes_256_decrypt(const std::string& ciphertext_with_iv, const std::string& secret) {
    if (ciphertext_with_iv.length() < 16 + 1) return "";
    auto key = derive_aes_key(secret);
    unsigned char iv[16];
    memcpy(iv, ciphertext_with_iv.data(), 16);

    const unsigned char* actual_ciphertext = (const unsigned char*)ciphertext_with_iv.data() + 16;
    int ciphertext_len = ciphertext_with_iv.length() - 16;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv);

    std::vector<unsigned char> decrypted(ciphertext_len + EVP_MAX_BLOCK_LENGTH);
    int len, decrypted_len;
    if (EVP_DecryptUpdate(ctx, decrypted.data(), &len, actual_ciphertext, ciphertext_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    decrypted_len = len;
    if (EVP_DecryptFinal_ex(ctx, decrypted.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return "";
    }
    decrypted_len += len;
    EVP_CIPHER_CTX_free(ctx);

    return std::string((char*)decrypted.data(), decrypted_len);
}


// Convert string to hex for safe file storage
static std::string to_hex(const std::string& input) {
    std::ostringstream oss;
    for (unsigned char c : input) {
        oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(c);
    }
    return oss.str();
}

// Convert hex back to string
static std::string from_hex(const std::string& hex) {
    std::string output;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byte = hex.substr(i, 2);
        output += static_cast<char>(std::stoi(byte, nullptr, 16));
    }
    return output;
}

Rate_Limiter::Rate_Limiter(int max_req, int window_sec) 
    : max_requests(max_req), time_window_seconds(window_sec), global_throttle_multiplier(1.0f) {}

void Rate_Limiter::clean_expired_timestamps(std::deque<std::chrono::steady_clock::time_point>& timestamps, 
                                            const std::chrono::steady_clock::time_point& now) {
    while(!timestamps.empty() && 
          std::chrono::duration_cast<std::chrono::seconds>(now - timestamps.front()).count() >= time_window_seconds) {
        timestamps.pop_front();
    }
}

bool Rate_Limiter::is_request_allowed(const std::string& user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();
    auto& timestamps = user_data[user_id].timestamps;
    
    clean_expired_timestamps(timestamps, now);
    
    int effective_limit = static_cast<int>(max_requests * global_throttle_multiplier);
    if (effective_limit < 1) effective_limit = 1; // Min 1 req/window even when throttled

    if (timestamps.size() < static_cast<size_t>(effective_limit)) {
        timestamps.push_back(now);
        return true;
    }
    return false;
}

int Rate_Limiter::get_remaining(const std::string& user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();
    
    auto it = user_data.find(user_id);
    if (it == user_data.end()) return max_requests;

    auto& timestamps = it->second.timestamps;
    clean_expired_timestamps(timestamps, now);

    int effective_limit = static_cast<int>(max_requests * global_throttle_multiplier);
    if (effective_limit < 1) effective_limit = 1;

    return std::max(0, effective_limit - static_cast<int>(timestamps.size()));
}

int Rate_Limiter::get_reset_time(const std::string& user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();
    
    auto it = user_data.find(user_id);
    if (it == user_data.end() || it->second.timestamps.empty()) return 0;

    clean_expired_timestamps(it->second.timestamps, now);
    if (it->second.timestamps.empty()) return 0;

    auto oldest = it->second.timestamps.front();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - oldest).count();
    
    return std::max(0, time_window_seconds - static_cast<int>(elapsed));
}

void Rate_Limiter::clear_user(const std::string& user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    user_data.erase(user_id);
}

void Rate_Limiter::save_to_file(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream outFile(filename);
    if (!outFile) return;

    for (auto const& pair : user_data) {
        const auto& id = pair.first;
        const auto& data = pair.second;
        if (data.timestamps.empty()) continue;
        
        // Build plaintext line
        std::ostringstream line;
        line << id << " " << data.timestamps.size();
        for (const auto& tp : data.timestamps) {
            line << " " << tp.time_since_epoch().count();
        }
        
        // Encrypt and write as hex
        std::string encrypted = aes_256_encrypt(line.str(), ENCRYPTION_KEY);
        outFile << to_hex(encrypted) << "\n";
    }

}

void Rate_Limiter::load_from_file(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ifstream inFile(filename);
    if (!inFile) return;

    std::string hex_line;
    while (std::getline(inFile, hex_line)) {
        if (hex_line.empty()) continue;
        
        try {
            // Decrypt from hex to binary string, then perform AES decryption
            std::string binary_cipher = from_hex(hex_line);
            std::string decrypted = aes_256_decrypt(binary_cipher, ENCRYPTION_KEY);
            if (decrypted.empty()) continue;

            std::istringstream iss(decrypted);
            
            std::string id;
            size_t count;
            if (!(iss >> id >> count)) continue;
            
            std::deque<std::chrono::steady_clock::time_point> loaded_stamps;
            for (size_t i = 0; i < count; ++i) {
                long long ticks;
                if (iss >> ticks) {
                    loaded_stamps.push_back(
                        std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration(ticks))
                    );
                }
            }
            user_data[id].timestamps = loaded_stamps;
        } catch (...) {
            // Skip corrupted lines
            continue;
        }
    }
}

void Rate_Limiter::set_global_throttle(float multiplier) {
    if (multiplier < 0.0f) multiplier = 0.0f;
    if (multiplier > 1.0f) multiplier = 1.0f;
    
    std::lock_guard<std::mutex> lock(mutex_);
    global_throttle_multiplier = multiplier;
    std::cout << "[SYSTEM] Global Throttle Multiplier set to " << multiplier << "\n";
}

float Rate_Limiter::get_global_throttle() const {
    return global_throttle_multiplier;
}