// Configuration file for Chat System
// Modify these settings as needed

#ifndef CHAT_CONFIG_H
#define CHAT_CONFIG_H

// Network Configuration
#define DEFAULT_SERVER_IP "127.0.0.1"
#define DEFAULT_SERVER_PORT 8888

// Timing Configuration
#define SOCKET_TIMEOUT_MS 5000
#define PING_INTERVAL_MS 30000

// Message Configuration
#define MAX_MESSAGE_LENGTH 65536
#define MAX_USERNAME_LENGTH 50
#define MAX_PASSWORD_LENGTH 100

// UI Configuration
#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define MIN_WINDOW_WIDTH 600
#define MIN_WINDOW_HEIGHT 400

// Server Configuration
#define MAX_CONCURRENT_CLIENTS 100
#define LISTEN_BACKLOG 10

// File paths (relative to working directory)
#define DATA_DIRECTORY "data"
#define USERS_DB_FILE "data/users.txt"
#define CHAT_LOG_FILE "data/chat.log"

// Enable debug logging
#define ENABLE_DEBUG_LOGGING 1

// Colors (Discord-like theme)
#define COLOR_PRIMARY "#7289DA"        // Blue
#define COLOR_SECONDARY "#23272A"      // Dark gray
#define COLOR_BACKGROUND "#2C2F33"     // Darker gray
#define COLOR_SUCCESS "#43B581"        // Green
#define COLOR_ERROR "#F04747"          // Red
#define COLOR_TEXT "#FFFFFF"           // White

#endif // CHAT_CONFIG_H
