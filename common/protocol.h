#pragma once

#include <string>
#include <cstdint>

#ifdef ERROR
#undef ERROR
#endif

// Message types
enum class MessageType : uint8_t {
    // Authentication
    REGISTER = 0x01,
    LOGIN = 0x02,
    LOGOUT = 0x03,
    
    // Chat operations
    SEND_MESSAGE = 0x10,
    RECEIVE_MESSAGE = 0x11,
    GET_ONLINE_USERS = 0x20,
    ONLINE_USERS_LIST = 0x21,
    USER_ONLINE = 0x22,
    USER_OFFLINE = 0x23,
    
    // General
    SUCCESS = 0xF0,
    ERROR = 0xF1,
    PING = 0xFE,
    PONG = 0xFF
};

// Message structure
struct MessageHeader {
    uint32_t length;      // Total message length (including header)
    MessageType type;
    uint8_t reserved1;
    uint16_t reserved2;
};

const int HEADER_SIZE = sizeof(MessageHeader);
const int MAX_MESSAGE_SIZE = 1024 * 64;  // 64 KB
const int MAX_USERNAME_SIZE = 50;

// Server configuration
const char* const SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8888;
const int MAX_CLIENTS = 100;
const int LISTEN_BACKLOG = 10;

// File paths
const char* const USERS_FILE = "data/users.txt";
const char* const LOG_FILE = "data/chat.log";
const char* const DATA_DIR = "data";
