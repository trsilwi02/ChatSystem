#pragma once

#include <winsock2.h>
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>

using ReceiveCallback = std::function<void(const std::vector<uint8_t>&)>;
using DisconnectCallback = std::function<void()>;

class SocketClient {
public:
    SocketClient();
    ~SocketClient();

    bool Connect(const std::string& ip, int port,
                 ReceiveCallback recv_callback = nullptr,
                 DisconnectCallback disc_callback = nullptr);
    void Disconnect();
    bool IsConnected() const;

    bool SendMessage(const std::vector<uint8_t>& data);

    void SetReceiveCallback(ReceiveCallback callback);
    void SetDisconnectCallback(DisconnectCallback callback);

private:
    void ReceiveLoop();
    bool RecvAll(char* buf, int len);          // <-- thêm mới
    bool ReceiveFullMessage(std::vector<uint8_t>& buffer);

    SOCKET socket_;
    bool is_connected_;
    bool should_stop_;

    ReceiveCallback receive_callback_;
    DisconnectCallback disconnect_callback_;

    std::thread receive_thread_;
    mutable std::mutex socket_mutex_;
};