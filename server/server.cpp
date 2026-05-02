#include "server.h"
#include "../common/message.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

Server::Server(const std::string& ip, int port)
    : ip_(ip), port_(port), listen_socket_(INVALID_SOCKET), is_running_(false) {
    user_manager_ = std::make_unique<UserManager>();
}

Server::~Server() {
    Stop();
}

bool Server::Start() {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        std::cerr << "WSAStartup failed\n";
        return false;
    }
    
    listen_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket_ == INVALID_SOCKET) {
        std::cerr << "socket() failed\n";
        WSACleanup();
        return false;
    }
    
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    inet_pton(AF_INET, ip_.c_str(), &server_addr.sin_addr.s_addr);
    
    if (bind(listen_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "bind() failed\n";
        closesocket(listen_socket_);
        WSACleanup();
        return false;
    }
    
    if (listen(listen_socket_, LISTEN_BACKLOG) == SOCKET_ERROR) {
        std::cerr << "listen() failed\n";
        closesocket(listen_socket_);
        WSACleanup();
        return false;
    }
    
    is_running_ = true;
    std::cout << "Server started on " << ip_ << ":" << port_ << "\n";
    
    std::thread(&Server::AcceptClients, this).detach();
    return true;
}

void Server::Stop() {
    is_running_ = false;
    
    if (listen_socket_ != INVALID_SOCKET) {
        closesocket(listen_socket_);
    }
    
    {
        std::lock_guard<std::mutex> lock(clients_mutex_);
        for (auto& client : clients_) {
            client.second->Stop();
        }
        clients_.clear();
    }
    
    WSACleanup();
    std::cout << "Server stopped\n";
}

bool Server::IsRunning() const {
    return is_running_;
}

void Server::AcceptClients() {
    while (is_running_) {
        SOCKET client_socket = accept(listen_socket_, nullptr, nullptr);
        if (client_socket == INVALID_SOCKET) {
            if (is_running_) {
                std::cerr << "accept() failed\n";
            }
            break;
        }
        
        auto handler = std::make_shared<ClientHandler>(client_socket, user_manager_.get());
        
        handler->SetMessageCallback([this](const std::string& from, const std::vector<uint8_t>& msg) {
            OnClientMessage(from, msg);
        });
        
        handler->SetDisconnectCallback([this](const std::string& username) {
            OnClientDisconnect(username);
        });
        
        {
            std::lock_guard<std::mutex> lock(clients_mutex_);
            clients_[std::to_string(reinterpret_cast<uintptr_t>(client_socket))] = handler;
        }
        
        handler->Start();
    }
}

void Server::OnClientMessage(const std::string& from_client, const std::vector<uint8_t>& message) {
    MessageType type = Message::GetMessageType(message);
    
    if (type == MessageType::SEND_MESSAGE) {
        std::string from, to, content;
        if (Message::ParseChatMessage(message, from, to, content)) {
            // Send message to recipient if online
            std::lock_guard<std::mutex> lock(clients_mutex_);
            for (auto& client : clients_) {
                if (client.second->GetUsername() == to && client.second->IsConnected()) {
                    auto msg = Message::CreateChatMessage(from, to, content);
                    client.second->SendMessage(msg);
                    break;
                }
            }
        }
    }
}

void Server::OnClientDisconnect(const std::string& username) {
    // Notify all clients that user went offline
    auto msg = Message::CreateUserOfflineMessage(username);
    BroadcastMessage(msg, username);
    
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (auto it = clients_.begin(); it != clients_.end(); ++it) {
        if (it->second->GetUsername() == username) {
            clients_.erase(it);
            break;
        }
    }
}

void Server::BroadcastMessage(const std::vector<uint8_t>& message, const std::string& exclude_user) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (auto& client : clients_) {
        if (client.second->IsConnected() && client.second->GetUsername() != exclude_user) {
            client.second->SendMessage(message);
        }
    }
}
