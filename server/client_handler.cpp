#include "client_handler.h"
#include "user_manager.h"
#include "../common/message.h"
#include <thread>
#include <iostream>

ClientHandler::ClientHandler(SOCKET socket, UserManager* user_manager)
    : socket_(socket), user_manager_(user_manager), is_connected_(true), 
      is_authenticated_(false), should_stop_(false) {
}

ClientHandler::~ClientHandler() {
    Stop();
}

void ClientHandler::Start() {
    std::thread(&ClientHandler::HandleClient, this).detach();
}

void ClientHandler::Stop() {
    should_stop_ = true;
    if (is_connected_) {
        closesocket(socket_);
        is_connected_ = false;
    }
}

void ClientHandler::SetMessageCallback(MessageCallback callback) {
    message_callback_ = callback;
}

void ClientHandler::SetDisconnectCallback(ClientDisconnectCallback callback) {
    disconnect_callback_ = callback;
}

bool ClientHandler::SendMessage(const std::vector<uint8_t>& data) {
    if (!is_connected_) return false;
    
    int sent = send(socket_, reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()), 0);
    return sent == static_cast<int>(data.size());
}

std::string ClientHandler::GetUsername() const {
    return username_;
}

bool ClientHandler::IsConnected() const {
    return is_connected_;
}

bool ClientHandler::IsAuthenticated() const {
    return is_authenticated_;
}

bool ClientHandler::ReceiveFullMessage(std::vector<uint8_t>& buffer) {
    buffer.clear();
    std::vector<uint8_t> header(HEADER_SIZE);
    
    // Receive header
    int received = recv(socket_, reinterpret_cast<char*>(header.data()), HEADER_SIZE, MSG_WAITALL);
    if (received != HEADER_SIZE) {
        return false;
    }
    
    // Parse message length
    uint32_t length = (static_cast<uint32_t>(header[0]) << 24) |
                      (static_cast<uint32_t>(header[1]) << 16) |
                      (static_cast<uint32_t>(header[2]) << 8) |
                      static_cast<uint32_t>(header[3]);
    
    if (length < HEADER_SIZE || length > MAX_MESSAGE_SIZE) {
        return false;
    }
    
    buffer.insert(buffer.end(), header.begin(), header.end());
    
    // Receive payload
    std::vector<uint8_t> payload(length - HEADER_SIZE);
    received = recv(socket_, reinterpret_cast<char*>(payload.data()), static_cast<int>(payload.size()), MSG_WAITALL);
    if (received != static_cast<int>(payload.size())) {
        return false;
    }
    
    buffer.insert(buffer.end(), payload.begin(), payload.end());
    return true;
}

void ClientHandler::HandleClient() {
    std::cout << "Client connected\n";
    
    while (is_connected_ && !should_stop_) {
        std::vector<uint8_t> message;
        
        if (!ReceiveFullMessage(message)) {
            is_connected_ = false;
            break;
        }
        
        ProcessMessage(message);
    }
    
    // Cleanup
    if (is_authenticated_) {
        user_manager_->SetUserOffline(username_);
        if (disconnect_callback_) {
            disconnect_callback_(username_);
        }
    }
    
    closesocket(socket_);
    is_connected_ = false;
    std::cout << "Client disconnected: " << username_ << "\n";
}

void ClientHandler::ProcessMessage(const std::vector<uint8_t>& data) {
    MessageType type = Message::GetMessageType(data);
    
    switch (type) {
        case MessageType::REGISTER: {
            std::string username, password;
            if (Message::ParseRegisterMessage(data, username, password)) {
                bool success = user_manager_->RegisterUser(username, password);
                auto response = success ? 
                    Message::CreateSuccessMessage("Registration successful") :
                    Message::CreateErrorMessage("Username already exists");
                SendMessage(response);
            }
            break;
        }
        case MessageType::LOGIN: {
            std::string username, password;
            if (Message::ParseLoginMessage(data, username, password)) {
                bool success = user_manager_->LoginUser(username, password);
                if (success) {
                    username_ = username;
                    is_authenticated_ = true;
                    user_manager_->SetUserOnline(username);
                    auto response = Message::CreateSuccessMessage("Login successful");
                    SendMessage(response);
                    
                    if (message_callback_) {
                        message_callback_(username_, data);
                    }
                } else {
                    auto response = Message::CreateErrorMessage("Invalid username or password");
                    SendMessage(response);
                }
            }
            break;
        }
        case MessageType::LOGOUT: {
            std::string username;
            if (Message::ParseLogoutMessage(data, username)) {
                user_manager_->SetUserOffline(username);
                is_authenticated_ = false;
                auto response = Message::CreateSuccessMessage("Logout successful");
                SendMessage(response);
                
                if (disconnect_callback_) {
                    disconnect_callback_(username);
                }
            }
            break;
        }
        case MessageType::GET_ONLINE_USERS: {
            auto users = user_manager_->GetOnlineUsers();
            auto response = Message::CreateOnlineUsersListMessage(users);
            SendMessage(response);
            break;
        }
        case MessageType::SEND_MESSAGE: {
            if (message_callback_) {
                message_callback_(username_, data);
            }
            break;
        }
        default:
            break;
    }
}
