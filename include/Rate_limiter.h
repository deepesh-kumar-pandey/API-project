#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

#include <unordered_map>
#include <string>
#include <chrono>
#include <deque>
#include <mutex>

class Rate_Limiter {
public:
    // --- Constructor ---
    Rate_Limiter(int max_req, int window_sec);

    // --- Public Interface ---
    bool is_request_allowed(const std::string& user_id);
    int get_remaining(const std::string& user_id);
    int get_reset_time(const std::string& user_id);
    void clear_user(const std::string& user_id);

    // --- File Operations ---
    void save_to_file(const std::string& filename);
    void load_from_file(const std::string& filename);

private:
    struct UserData {
        std::deque<std::chrono::steady_clock::time_point> timestamps;
    };

    std::mutex mutex_;
    int max_requests;
    int time_window_seconds;
    std::unordered_map<std::string, UserData> user_data;

    // --- Private Helper ---
    void clean_expired_timestamps(std::deque<std::chrono::steady_clock::time_point>& timestamps, 
                                 const std::chrono::steady_clock::time_point& now);
};

#endif