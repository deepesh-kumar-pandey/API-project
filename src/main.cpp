#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include "../include/Rate_limiter.h"

using namespace std;

void display_help() {
    cout << "\n--- Gatekeeper CLI Commands ---\n";
    cout << "1. check <user_id>  - Log a request\n";
    cout << "2. status <user_id> - View remaining hits\n";
    cout << "3. clear <user_id>  - Reset user limits\n";
    cout << "4. help             - Show this menu\n";
    cout << "5. exit             - Save and quit\n";
    cout << "-------------------------------\n\n";
}

void print_header() {
    cout << "========================================\n";
    cout << "        Gatekeeper CLI         \n";
    cout << "========================================\n";
    cout << " Storage: limiter_db.txt (encrypted)\n";
    cout << " Commands: check <id>, status <id>, exit\n";
    cout << "========================================\n";
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

    cout << "=== Rate Limiter Configuration ===\n";
    cout << "Enter Max Requests allowed: ";
    cin >> max_req;
    cout << "Enter Time Window (in seconds): ";
    cin >> win_sec;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Rate_Limiter limiter(max_req, win_sec);
    limiter.load_from_file("limiter_db.txt");

    print_header();
    seed_data(limiter);

    cout << "\nRate Limiter System Active (" << max_req << " req / " << win_sec << "s)\n";
    display_help();

    string line;
    while (true) {
        cout << ">> ";
        if (!getline(cin, line)) break;

        stringstream ss(line);
        string command, userId;
        ss >> command >> userId;

        if (command == "exit") {
            limiter.save_to_file("limiter_db.txt");
            cout << "Data saved. Goodbye!\n";
            break;
        } 
        else if (command == "check") {
            if (userId.empty()) { cout << "Error: Provide a user ID.\n"; continue; }
            if (limiter.is_request_allowed(userId)) {
                cout << "[ALLOWED] Request logged for " << userId 
                     << " | Remaining: " << limiter.get_remaining(userId) << "/" << max_req << "\n";
            } else {
                cout << "[DENIED] Rate limit exceeded for " << userId << ". Try again in " 
                     << limiter.get_reset_time(userId) << "s.\n";
            }
        } 
        else if (command == "status") {
            if (userId.empty()) { cout << "Error: Provide a user ID.\n"; continue; }
            int remaining = limiter.get_remaining(userId);
            cout << "User " << userId << " has " << remaining << " requests left.\n";
        }
        else if (command == "clear") {
            if (userId.empty()) { cout << "Error: Provide a user ID.\n"; continue; }
            limiter.clear_user(userId);
            cout << "History cleared for " << userId << "\n";
        }
        else if (command == "help") {
            display_help();
        }
        else if (!command.empty()) {
            cout << "Unknown command: " << command << ". Type 'help' for options.\n";
        }
    }

    return 0;
}