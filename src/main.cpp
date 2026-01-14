#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include "../include/Rate_limiter.h"

using namespace std;

void display_help() {
    cout << "\n--- Gatekeeper CLI Commands ---" << "\n";
    cout << "1. check <user_id>  - Log a request" << "\n";
    cout << "2. status <user_id> - View remaining hits" << "\n";
    cout << "3. clear <user_id>  - Reset user limits" << "\n";
    cout << "4. help            - Show this menu" << "\n";
    cout << "5. exit            - Save and quit" << "\n";
    cout << "-------------------------------\n" << endl;
}

void print_header() {
    cout << "========================================" << "\n";
    cout << "        Gatekeeper CLI         " << "\n";
    cout << "========================================" << "\n";
    cout << " Storage: limiter_db.txt" << "\n";
    cout << " Commands: check <id>, status <id>, exit" << "\n";
    cout << "========================================" << "\n";
}

void seed_data(Rate_Limiter& limiter) {
    vector<string> dummy_users = {"alex", "bob", "gemini", "dev_user"};
    for(const string& user : dummy_users) {
        for(int i = 0; i < 3; i++) {
            limiter.is_request_allowed(user);
        }
    }
    cout << ">> System seeded with " << dummy_users.size() << " dummy users.\n";
}

int main() {
    int max_req, win_sec;

    std::cout << "=== Rate Limiter Configuration ===" << std::endl;
    std::cout << "Enter Max Requests allowed: ";
    std::cin >> max_req;
    std::cout << "Enter Time Window (in seconds): ";
    std::cin >> win_sec;
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    Rate_Limiter limiter(max_req, win_sec);
    limiter.load_from_file("limiter_db.txt");

    print_header();
    seed_data(limiter);

    std::cout << "\nRate Limiter System Active (" << max_req << " req / " << win_sec << "s)" << std::endl;
    display_help();

    std::string line;
    while (true) {
        std::cout << ">> ";
        if (!std::getline(std::cin, line)) break;

        std::stringstream ss(line);
        std::string command, userId;
        ss >> command >> userId;

        if (command == "exit") {
            limiter.save_to_file("limiter_db.txt");
            std::cout << "Data saved. Goodbye!" << std::endl;
            break;
        } 
        else if (command == "check") {
            if (userId.empty()) {
                std::cout << "Error: Provide a user ID." << std::endl;
                continue;
            }
            if (limiter.is_request_allowed(userId)) {
                std::cout << "[ALLOWED] Request logged for " << userId << std::endl;
            } else {
                std::cout << "[DENIED] Rate limit exceeded for " << userId << ". Try again in " 
                          << limiter.get_reset_time(userId) << "s." << std::endl;
            }
        } 
        else if (command == "status") {
            if (userId.empty()) { std::cout << "Error: Provide a user ID." << std::endl; continue; }
            int remaining = limiter.get_remaining(userId);
            std::cout << "User " << userId << " has " << remaining << " requests left." << std::endl;
        }
        else if (command == "clear") {
            if (userId.empty()) { std::cout << "Error: Provide a user ID." << std::endl; continue; }
            limiter.clear_user(userId);
            std::cout << "History cleared for " << userId << std::endl;
        }
        else if (command == "help") {
            display_help();
        }
        else if (!command.empty()) {
            std::cout << "Unknown command: " << command << ". Type 'help' for options." << std::endl;
        }
    }

    return 0;
}