#pragma once

#include "protocol.h"
#include <vector>
#include <string>
#include <memory>

class Message {
public:
    Message();
    explicit Message(MessageType type);
    
    static std::vector<uint8_t> CreateRegisterMessage(const std::string& username, const std::string& password);
    static std::vector<uint8_t> CreateLoginMessage(const std::string& username, const std::string& password);
    static std::vector<uint8_t> CreateLogoutMessage(const std::string& username);
    static std::vector<uint8_t> CreateChatMessage(const std::string& from, const std::string& to, const std::string& content);
    static std::vector<uint8_t> CreateGetOnlineUsersMessage();
    static std::vector<uint8_t> CreateOnlineUsersListMessage(const std::vector<std::string>& users);
    static std::vector<uint8_t> CreateSuccessMessage(const std::string& detail = "");
    static std::vector<uint8_t> CreateErrorMessage(const std::string& error);
    static std::vector<uint8_t> CreateUserOnlineMessage(const std::string& username);
    static std::vector<uint8_t> CreateUserOfflineMessage(const std::string& username);
    
    // Parsing
    static MessageType GetMessageType(const std::vector<uint8_t>& data);
    static bool ParseRegisterMessage(const std::vector<uint8_t>& data, std::string& username, std::string& password);
    static bool ParseLoginMessage(const std::vector<uint8_t>& data, std::string& username, std::string& password);
    static bool ParseLogoutMessage(const std::vector<uint8_t>& data, std::string& username);
    static bool ParseChatMessage(const std::vector<uint8_t>& data, std::string& from, std::string& to, std::string& content);
    static bool ParseErrorMessage(const std::vector<uint8_t>& data, std::string& error);
    static bool ParseOnlineUsersList(const std::vector<uint8_t>& data, std::vector<std::string>& users);
    static bool ParseUserStatusMessage(const std::vector<uint8_t>& data, std::string& username);
    
private:
    static std::vector<uint8_t> EncodeString(const std::string& str);
    static bool DecodeString(const std::vector<uint8_t>& data, size_t& offset, std::string& result);
};
