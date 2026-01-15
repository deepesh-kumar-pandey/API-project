#include "../include/Rate_limiter.h"
#include <fstream>
#include <algorithm>
using namespace std;  

Rate_Limiter::Rate_Limiter(int max_req, int window_sec) 
    : max_requests(max_req), time_window_seconds(window_sec) {}

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

    if (timestamps.size() < static_cast<size_t>(max_requests)) {
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

    return std::max(0, max_requests - static_cast<int>(timestamps.size()));
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
        outFile << id << " " << data.timestamps.size();
        for (const auto& tp : data.timestamps) {
            outFile << " " << tp.time_since_epoch().count();
        }
        outFile << "\n";
    }
}

void Rate_Limiter::load_from_file(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ifstream inFile(filename);
    if (!inFile) return;

    std::string id;
    size_t count;
    while (inFile >> id >> count) {
        std::deque<std::chrono::steady_clock::time_point> loaded_stamps;
        for (size_t i = 0; i < count; ++i) {
            long long ticks;
            if (inFile >> ticks) {
                loaded_stamps.push_back(
                    std::chrono::steady_clock::time_point(std::chrono::steady_clock::duration(ticks))
                );
            }
        }
        user_data[id].timestamps = loaded_stamps;
    }
}