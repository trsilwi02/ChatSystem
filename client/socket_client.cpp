#include "socket_client.h"
#include "../common/message.h"
#include "../common/protocol.h"
#include <iostream>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

SocketClient::SocketClient()
    : socket_(INVALID_SOCKET), is_connected_(false), should_stop_(false) {
}

SocketClient::~SocketClient() {
    Disconnect();
}

bool SocketClient::Connect(const std::string& ip, int port,
                           ReceiveCallback recv_callback,
                           DisconnectCallback disc_callback) {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return false;
    }

    socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_ == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr.s_addr);

    if (connect(socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(socket_);
        WSACleanup();
        return false;
    }

    // Set callbacks BEFORE starting receive thread to avoid race condition
    if (recv_callback) {
        receive_callback_ = recv_callback;
    }
    if (disc_callback) {
        disconnect_callback_ = disc_callback;
    }

    is_connected_ = true;
    should_stop_ = false;

    receive_thread_ = std::thread(&SocketClient::ReceiveLoop, this);
    return true;
}

void SocketClient::Disconnect() {
    should_stop_ = true;
    is_connected_ = false;
    
    std::lock_guard<std::mutex> lock(socket_mutex_);
    if (socket_ != INVALID_SOCKET) {
        closesocket(socket_);
        socket_ = INVALID_SOCKET;
    }
    
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
    
    WSACleanup();
}

bool SocketClient::IsConnected() const {
    return is_connected_;
}

bool SocketClient::SendMessage(const std::vector<uint8_t>& data) {
    if (!is_connected_) return false;
    
    std::lock_guard<std::mutex> lock(socket_mutex_);
    if (socket_ == INVALID_SOCKET) return false;
    
    int sent = send(socket_, reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()), 0);
    return sent == static_cast<int>(data.size());
}

void SocketClient::SetReceiveCallback(ReceiveCallback callback) {
    receive_callback_ = callback;
}

void SocketClient::SetDisconnectCallback(DisconnectCallback callback) {
    disconnect_callback_ = callback;
}

bool SocketClient::ReceiveFullMessage(std::vector<uint8_t>& buffer) {
    buffer.clear();
    std::vector<uint8_t> header(HEADER_SIZE);
    
    std::lock_guard<std::mutex> lock(socket_mutex_);
    if (socket_ == INVALID_SOCKET) return false;
    
    int received = recv(socket_, reinterpret_cast<char*>(header.data()), HEADER_SIZE, MSG_WAITALL);
    if (received != HEADER_SIZE) {
        return false;
    }
    
    uint32_t length = (static_cast<uint32_t>(header[0]) << 24) |
                      (static_cast<uint32_t>(header[1]) << 16) |
                      (static_cast<uint32_t>(header[2]) << 8) |
                      static_cast<uint32_t>(header[3]);
    
    if (length < HEADER_SIZE || length > MAX_MESSAGE_SIZE) {
        return false;
    }
    
    buffer.insert(buffer.end(), header.begin(), header.end());
    
    std::vector<uint8_t> payload(length - HEADER_SIZE);
    received = recv(socket_, reinterpret_cast<char*>(payload.data()), static_cast<int>(payload.size()), MSG_WAITALL);
    if (received != static_cast<int>(payload.size())) {
        return false;
    }
    
    buffer.insert(buffer.end(), payload.begin(), payload.end());
    return true;
}

void SocketClient::ReceiveLoop() {
    while (!should_stop_ && is_connected_) {
        std::vector<uint8_t> message;
        
        if (!ReceiveFullMessage(message)) {
            is_connected_ = false;
            if (disconnect_callback_) {
                disconnect_callback_();
            }
            break;
        }
        
        if (receive_callback_) {
            receive_callback_(message);
        }
    }
}
