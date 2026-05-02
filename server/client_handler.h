#pragma once

#include <winsock2.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>

class ClientHandler;
class UserManager;

// Forward declaration
using MessageCallback = std::function<void(const std::string&, const std::vector<uint8_t>&)>;
using ClientDisconnectCallback = std::function<void(const std::string&)>;

class ClientHandler {
public:
    ClientHandler(SOCKET socket, UserManager* user_manager);
    ~ClientHandler();
    
    // Start handling client
    void Start();
    void Stop();
    
    // Callbacks
    void SetMessageCallback(MessageCallback callback);
    void SetDisconnectCallback(ClientDisconnectCallback callback);
    
    // Send message to client
    bool SendMessage(const std::vector<uint8_t>& data);
    
    // Get client info
    std::string GetUsername() const;
    bool IsConnected() const;
    bool IsAuthenticated() const;
    
private:
    void HandleClient();
    bool ReceiveFullMessage(std::vector<uint8_t>& buffer);
    void ProcessMessage(const std::vector<uint8_t>& data);
    
    SOCKET socket_;
    UserManager* user_manager_;
    std::string username_;
    bool is_connected_;
    bool is_authenticated_;
    bool should_stop_;
    
    MessageCallback message_callback_;
    ClientDisconnectCallback disconnect_callback_;
};
