#include "message.h"
#include <cstring>
#include <algorithm>

Message::Message() {}

Message::Message(MessageType type) {}

std::vector<uint8_t> Message::EncodeString(const std::string& str) {
    std::vector<uint8_t> result;
    uint16_t len = static_cast<uint16_t>(str.length());
    result.push_back(static_cast<uint8_t>(len >> 8));
    result.push_back(static_cast<uint8_t>(len & 0xFF));
    result.insert(result.end(), str.begin(), str.end());
    return result;
}

bool Message::DecodeString(const std::vector<uint8_t>& data, size_t& offset, std::string& result) {
    if (offset + 2 > data.size()) return false;
    
    uint16_t len = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
    offset += 2;
    
    if (offset + len > data.size()) return false;
    
    result = std::string(data.begin() + offset, data.begin() + offset + len);
    offset += len;
    return true;
}

std::vector<uint8_t> Message::CreateRegisterMessage(const std::string& username, const std::string& password) {
    std::vector<uint8_t> payload;
    auto user = EncodeString(username);
    auto pass = EncodeString(password);
    payload.insert(payload.end(), user.begin(), user.end());
    payload.insert(payload.end(), pass.begin(), pass.end());
    
    std::vector<uint8_t> result;
    uint32_t length = HEADER_SIZE + payload.size();
    result.push_back(static_cast<uint8_t>(length >> 24));
    result.push_back(static_cast<uint8_t>((length >> 16) & 0xFF));
    result.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>(length & 0xFF));
    result.push_back(static_cast<uint8_t>(MessageType::REGISTER));
    result.push_back(0);
    result.push_back(0);
    result.push_back(0);
    result.insert(result.end(), payload.begin(), payload.end());
    
    return result;
}

std::vector<uint8_t> Message::CreateLoginMessage(const std::string& username, const std::string& password) {
    std::vector<uint8_t> payload;
    auto user = EncodeString(username);
    auto pass = EncodeString(password);
    payload.insert(payload.end(), user.begin(), user.end());
    payload.insert(payload.end(), pass.begin(), pass.end());
    
    std::vector<uint8_t> result;
    uint32_t length = HEADER_SIZE + payload.size();
    result.push_back(static_cast<uint8_t>(length >> 24));
    result.push_back(static_cast<uint8_t>((length >> 16) & 0xFF));
    result.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>(length & 0xFF));
    result.push_back(static_cast<uint8_t>(MessageType::LOGIN));
    result.push_back(0);
    result.push_back(0);
    result.push_back(0);
    result.insert(result.end(), payload.begin(), payload.end());
    
    return result;
}

std::vector<uint8_t> Message::CreateLogoutMessage(const std::string& username) {
    auto payload = EncodeString(username);
    
    std::vector<uint8_t> result;
    uint32_t length = HEADER_SIZE + payload.size();
    result.push_back(static_cast<uint8_t>(length >> 24));
    result.push_back(static_cast<uint8_t>((length >> 16) & 0xFF));
    result.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>(length & 0xFF));
    result.push_back(static_cast<uint8_t>(MessageType::LOGOUT));
    result.push_back(0);
    result.push_back(0);
    result.push_back(0);
    result.insert(result.end(), payload.begin(), payload.end());
    
    return result;
}

std::vector<uint8_t> Message::CreateChatMessage(const std::string& from, const std::string& to, const std::string& content) {
    std::vector<uint8_t> payload;
    auto f = EncodeString(from);
    auto t = EncodeString(to);
    auto c = EncodeString(content);
    payload.insert(payload.end(), f.begin(), f.end());
    payload.insert(payload.end(), t.begin(), t.end());
    payload.insert(payload.end(), c.begin(), c.end());
    
    std::vector<uint8_t> result;
    uint32_t length = HEADER_SIZE + payload.size();
    result.push_back(static_cast<uint8_t>(length >> 24));
    result.push_back(static_cast<uint8_t>((length >> 16) & 0xFF));
    result.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>(length & 0xFF));
    result.push_back(static_cast<uint8_t>(MessageType::SEND_MESSAGE));
    result.push_back(0);
    result.push_back(0);
    result.push_back(0);
    result.insert(result.end(), payload.begin(), payload.end());
    
    return result;
}

std::vector<uint8_t> Message::CreateGetOnlineUsersMessage() {
    std::vector<uint8_t> result;
    uint32_t length = HEADER_SIZE;
    result.push_back(static_cast<uint8_t>(length >> 24));
    result.push_back(static_cast<uint8_t>((length >> 16) & 0xFF));
    result.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>(length & 0xFF));
    result.push_back(static_cast<uint8_t>(MessageType::GET_ONLINE_USERS));
    result.push_back(0);
    result.push_back(0);
    result.push_back(0);
    
    return result;
}

std::vector<uint8_t> Message::CreateOnlineUsersListMessage(const std::vector<std::string>& users) {
    std::vector<uint8_t> payload;
    uint16_t count = static_cast<uint16_t>(users.size());
    payload.push_back(static_cast<uint8_t>(count >> 8));
    payload.push_back(static_cast<uint8_t>(count & 0xFF));
    
    for (const auto& user : users) {
        auto u = EncodeString(user);
        payload.insert(payload.end(), u.begin(), u.end());
    }
    
    std::vector<uint8_t> result;
    uint32_t length = HEADER_SIZE + payload.size();
    result.push_back(static_cast<uint8_t>(length >> 24));
    result.push_back(static_cast<uint8_t>((length >> 16) & 0xFF));
    result.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>(length & 0xFF));
    result.push_back(static_cast<uint8_t>(MessageType::ONLINE_USERS_LIST));
    result.push_back(0);
    result.push_back(0);
    result.push_back(0);
    result.insert(result.end(), payload.begin(), payload.end());
    
    return result;
}

std::vector<uint8_t> Message::CreateSuccessMessage(const std::string& detail) {
    auto payload = EncodeString(detail);
    
    std::vector<uint8_t> result;
    uint32_t length = HEADER_SIZE + payload.size();
    result.push_back(static_cast<uint8_t>(length >> 24));
    result.push_back(static_cast<uint8_t>((length >> 16) & 0xFF));
    result.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>(length & 0xFF));
    result.push_back(static_cast<uint8_t>(MessageType::SUCCESS));
    result.push_back(0);
    result.push_back(0);
    result.push_back(0);
    result.insert(result.end(), payload.begin(), payload.end());
    
    return result;
}

std::vector<uint8_t> Message::CreateErrorMessage(const std::string& error) {
    auto payload = EncodeString(error);
    
    std::vector<uint8_t> result;
    uint32_t length = HEADER_SIZE + payload.size();
    result.push_back(static_cast<uint8_t>(length >> 24));
    result.push_back(static_cast<uint8_t>((length >> 16) & 0xFF));
    result.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>(length & 0xFF));
    result.push_back(static_cast<uint8_t>(MessageType::ERROR));
    result.push_back(0);
    result.push_back(0);
    result.push_back(0);
    result.insert(result.end(), payload.begin(), payload.end());
    
    return result;
}

std::vector<uint8_t> Message::CreateUserOnlineMessage(const std::string& username) {
    auto payload = EncodeString(username);
    
    std::vector<uint8_t> result;
    uint32_t length = HEADER_SIZE + payload.size();
    result.push_back(static_cast<uint8_t>(length >> 24));
    result.push_back(static_cast<uint8_t>((length >> 16) & 0xFF));
    result.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>(length & 0xFF));
    result.push_back(static_cast<uint8_t>(MessageType::USER_ONLINE));
    result.push_back(0);
    result.push_back(0);
    result.push_back(0);
    result.insert(result.end(), payload.begin(), payload.end());
    
    return result;
}

std::vector<uint8_t> Message::CreateUserOfflineMessage(const std::string& username) {
    auto payload = EncodeString(username);
    
    std::vector<uint8_t> result;
    uint32_t length = HEADER_SIZE + payload.size();
    result.push_back(static_cast<uint8_t>(length >> 24));
    result.push_back(static_cast<uint8_t>((length >> 16) & 0xFF));
    result.push_back(static_cast<uint8_t>((length >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>(length & 0xFF));
    result.push_back(static_cast<uint8_t>(MessageType::USER_OFFLINE));
    result.push_back(0);
    result.push_back(0);
    result.push_back(0);
    result.insert(result.end(), payload.begin(), payload.end());
    
    return result;
}

MessageType Message::GetMessageType(const std::vector<uint8_t>& data) {
    if (data.size() < HEADER_SIZE) return MessageType::ERROR;
    return static_cast<MessageType>(data[4]);
}

bool Message::ParseRegisterMessage(const std::vector<uint8_t>& data, std::string& username, std::string& password) {
    if (data.size() < HEADER_SIZE) return false;
    size_t offset = HEADER_SIZE;
    return DecodeString(data, offset, username) && DecodeString(data, offset, password);
}

bool Message::ParseLoginMessage(const std::vector<uint8_t>& data, std::string& username, std::string& password) {
    if (data.size() < HEADER_SIZE) return false;
    size_t offset = HEADER_SIZE;
    return DecodeString(data, offset, username) && DecodeString(data, offset, password);
}

bool Message::ParseLogoutMessage(const std::vector<uint8_t>& data, std::string& username) {
    if (data.size() < HEADER_SIZE) return false;
    size_t offset = HEADER_SIZE;
    return DecodeString(data, offset, username);
}

bool Message::ParseChatMessage(const std::vector<uint8_t>& data, std::string& from, std::string& to, std::string& content) {
    if (data.size() < HEADER_SIZE) return false;
    size_t offset = HEADER_SIZE;
    return DecodeString(data, offset, from) && DecodeString(data, offset, to) && DecodeString(data, offset, content);
}

bool Message::ParseErrorMessage(const std::vector<uint8_t>& data, std::string& error) {
    if (data.size() < HEADER_SIZE) return false;
    size_t offset = HEADER_SIZE;
    return DecodeString(data, offset, error);
}

bool Message::ParseOnlineUsersList(const std::vector<uint8_t>& data, std::vector<std::string>& users) {
    if (data.size() < HEADER_SIZE + 2) return false;
    size_t offset = HEADER_SIZE;
    uint16_t count = (static_cast<uint16_t>(data[offset]) << 8) | data[offset + 1];
    offset += 2;
    
    users.clear();
    for (uint16_t i = 0; i < count; i++) {
        std::string user;
        if (!DecodeString(data, offset, user)) return false;
        users.push_back(user);
    }
    return true;
}

bool Message::ParseUserStatusMessage(const std::vector<uint8_t>& data, std::string& username) {
    if (data.size() < HEADER_SIZE) return false;
    size_t offset = HEADER_SIZE;
    return DecodeString(data, offset, username);
}
