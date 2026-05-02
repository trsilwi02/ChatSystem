#pragma once

#include "client_handler.h"
#include "user_manager.h"
#include "../common/protocol.h"
#include <winsock2.h>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <string>

class Server {
public:
    Server(const std::string& ip, int port);
    ~Server();
    
    bool Start();
    void Stop();
    bool IsRunning() const;
    
private:
    void AcceptClients();
    void OnClientMessage(const std::string& from_client, const std::vector<uint8_t>& message);
    void OnClientDisconnect(const std::string& username);
    void BroadcastMessage(const std::vector<uint8_t>& message, const std::string& exclude_user = "");
    
    std::string ip_;
    int port_;
    SOCKET listen_socket_;
    bool is_running_;
    
    std::unique_ptr<UserManager> user_manager_;
    std::map<std::string, std::shared_ptr<ClientHandler>> clients_;
    std::mutex clients_mutex_;
};
