#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include "../include/Rate_limiter.h"
#include "../include/TrafficSniffer.h"


using namespace std;

void display_help() {
    cout << "\n--- Gatekeeper CLI Commands ---\n";
    cout << "1. check <user_id>  - Log a request\n";
    cout << "2. status <user_id> - View remaining hits\n";
    cout << "3. clear <user_id>  - Reset user limits\n";
    cout << "4. throttle <0.0-1.0> - Set global throttle multiplier\n";
    cout << "5. help             - Show this menu\n";
    cout << "5. sniff <port>     - Start automatic traffic detection\n";
    cout << "6. help             - Show this menu\n";
    cout << "7. exit             - Save and quit\n";

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

int main() {
    int max_req, win_sec;

    cout << "=== Rate Limiter Configuration ===\n";
    cout << "Enter Max Requests allowed: ";
    cin >> max_req;
    cout << "Enter Time Window (in seconds): ";
    cin >> win_sec;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Rate_Limiter limiter(max_req, win_sec);
    TrafficSniffer sniffer;
    limiter.load_from_file("limiter_db.txt");


    print_header();

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
        else if (command == "throttle") {
            float multiplier;
            if (!(ss >> multiplier)) {
                cout << "Error: Provide a multiplier (0.0 to 1.0).\n";
            } else {
                limiter.set_global_throttle(multiplier);
            }
        }
        else if (command == "sniff") {
            int port;
            if (!(ss >> port)) {
                cout << "Error: Provide a port number (e.g., 80).\n";
            } else {
                sniffer.stop_sniffing(); // Stop existing if any
                bool success = sniffer.start_sniffing(port, [&](const string& ip) {
                    if (limiter.is_request_allowed(ip)) {
                        cout << "[AUTO-DETECTED] [ALLOWED] Source: " << ip 
                             << " | Remaining: " << limiter.get_remaining(ip) << "/" << max_req << "\n";
                    } else {
                        cout << "[AUTO-DETECTED] [DENIED] Source: " << ip << ". Throttled.\n";
                    }
                });
                if (success) {
                    cout << "[SYSTEM] Auto-Detection started on port " << port << ".\n";
                } else {
                    cout << "[ERROR] Failed to start sniffing.\n";
                }
            }
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