#include "user_manager.h"
#include "../common/protocol.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <iostream>
#include <set>

namespace fs = std::filesystem;

UserManager::UserManager() {
    EnsureDataDirectory();
    LoadUsersFromFile();
}

UserManager::~UserManager() {
    SaveUsersToFile();
}

void UserManager::EnsureDataDirectory() {
    if (!fs::exists(DATA_DIR)) {
        fs::create_directories(DATA_DIR);
    }
}

void UserManager::LoadUsersFromFile() {
    std::lock_guard<std::mutex> lock(user_mutex_);
    users_.clear();

    std::ifstream file(USERS_FILE);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string username = line.substr(0, pos);
            std::string password = line.substr(pos + 1);
            if (!username.empty() && !password.empty()) {
                users_[username] = password;
            }
        }
    }
    file.close();
}

// Phiên bản KHÔNG lock — chỉ gọi khi đã giữ user_mutex_
void UserManager::SaveUsersToFileUnlocked() const {
    std::ofstream file(USERS_FILE);
    if (!file.is_open()) {
        std::cerr << "Failed to open users file for writing\n";
        return;
    }
    for (const auto& pair : users_) {
        file << pair.first << ":" << pair.second << "\n";
    }
    file.close();
}

// Phiên bản PUBLIC — tự lock
void UserManager::SaveUsersToFile() const {
    std::lock_guard<std::mutex> lock(user_mutex_);
    SaveUsersToFileUnlocked();
}

bool UserManager::RegisterUser(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) return false;

    std::lock_guard<std::mutex> lock(user_mutex_);

    if (users_.find(username) != users_.end()) {
        return false;  // User already exists
    }

    users_[username] = password;
    SaveUsersToFileUnlocked();  // ← dùng unlocked version, đang giữ lock rồi
    LogAction("REGISTER:" + username);
    return true;
}

bool UserManager::LoginUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(user_mutex_);

    auto it = users_.find(username);
    if (it == users_.end() || it->second != password) {
        return false;
    }

    online_users_.insert(username);
    LogAction("LOGIN:" + username);
    return true;
}

bool UserManager::LogoutUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(user_mutex_);

    online_users_.erase(username);
    LogAction("LOGOUT:" + username);
    return true;
}

bool UserManager::UserExists(const std::string& username) const {
    std::lock_guard<std::mutex> lock(user_mutex_);
    return users_.find(username) != users_.end();
}

std::vector<std::string> UserManager::GetOnlineUsers() const {
    std::lock_guard<std::mutex> lock(user_mutex_);
    return std::vector<std::string>(online_users_.begin(), online_users_.end());
}

bool UserManager::IsUserOnline(const std::string& username) const {
    std::lock_guard<std::mutex> lock(user_mutex_);
    return online_users_.find(username) != online_users_.end();
}

void UserManager::SetUserOnline(const std::string& username) {
    std::lock_guard<std::mutex> lock(user_mutex_);
    online_users_.insert(username);
}

void UserManager::SetUserOffline(const std::string& username) {
    std::lock_guard<std::mutex> lock(user_mutex_);
    online_users_.erase(username);
}

void UserManager::LogAction(const std::string& action) {
    std::ofstream file(LOG_FILE, std::ios::app);
    if (!file.is_open()) return;

    time_t now = time(nullptr);
    char timestamp[100];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

    file << "[" << timestamp << "] " << action << "\n";
    file.close();
}