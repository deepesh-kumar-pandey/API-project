#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

#include <unordered_map>
#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <deque>
#include <vector>
#include<mutex>

class Rate_Limiter {
public:
    Rate_Limiter(int max_req, int window_sec) 
        : max_requests(max_req), time_window_seconds(window_sec) {}

    bool is_request_allowed(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::steady_clock::now();
        auto& timestamps = user_data[user_id].timestamps;
        
        while(!timestamps.empty() && 
              std::chrono::duration_cast<std::chrono::seconds>(now - timestamps.front()).count() >= time_window_seconds) {
            timestamps.pop_front();
        }

        if (timestamps.size() < max_requests) {
            timestamps.push_back(now);
            return true;
        }

        return false;
    }

    int get_remaining(const std::string& user_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 1. If user doesn't exist, they have full capacity
    auto it = user_data.find(user_id);
    if (it == user_data.end()) {
        return max_requests;
    }

    auto now = std::chrono::steady_clock::now();
    auto& timestamps = it->second.timestamps;

    // 2. Clean up expired timestamps so the count is accurate
    while (!timestamps.empty() && 
           std::chrono::duration_cast<std::chrono::seconds>(now - timestamps.front()).count() >= time_window_seconds) {
        timestamps.pop_front();
    }

    // 3. Subtract current valid hits from the maximum allowed
    return std::max(0, max_requests - static_cast<int>(timestamps.size()));
}

    int get_reset_time(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::steady_clock::now();
        auto it = user_data.find(user_id);
        
        if (it == user_data.end() || it->second.timestamps.empty()) return 0;

        auto oldest = it->second.timestamps.front();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - oldest).count();
        
        return std::max(0, time_window_seconds - static_cast<int>(elapsed));
    }

    void save_to_file(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ofstream outFile(filename);
        if (!outFile) return;

        for (auto const& [id, data] : user_data) {
            if (data.timestamps.empty()) continue;
            outFile << id << " " << data.timestamps.size();
            for (const auto& tp : data.timestamps) {
                outFile << " " << tp.time_since_epoch().count();
            }
            outFile << "\n";
        }
    }

    void load_from_file(const std::string& filename) {
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
    
    void clear_user(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(mutex_);
        user_data.erase(user_id);
    }

private:
    struct UserData {
        std::deque<std::chrono::steady_clock::time_point> timestamps;
    };

    std::mutex mutex_;
    int max_requests;
    int time_window_seconds;
    std::unordered_map<std::string, UserData> user_data;
};

#endif