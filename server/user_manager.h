#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <mutex>

class UserManager {
public:
    UserManager();
    ~UserManager();

    // User operations
    bool RegisterUser(const std::string& username, const std::string& password);
    bool LoginUser(const std::string& username, const std::string& password);
    bool LogoutUser(const std::string& username);
    bool UserExists(const std::string& username) const;

    // Online users
    std::vector<std::string> GetOnlineUsers() const;
    bool IsUserOnline(const std::string& username) const;
    void SetUserOnline(const std::string& username);
    void SetUserOffline(const std::string& username);

    // Logging
    void LogAction(const std::string& action);

private:
    void LoadUsersFromFile();
    void SaveUsersToFile() const;          // public version — tự lock
    void SaveUsersToFileUnlocked() const;  // internal — gọi khi đã giữ lock
    void EnsureDataDirectory();

    mutable std::mutex user_mutex_;
    std::map<std::string, std::string> users_;  // username -> password
    std::set<std::string> online_users_;
};