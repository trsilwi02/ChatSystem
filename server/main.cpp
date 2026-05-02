#include "server.h"
#include "../common/protocol.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    Server server(SERVER_IP, SERVER_PORT);
    
    if (!server.Start()) {
        std::cerr << "Failed to start server\n";
        return 1;
    }
    
    std::cout << "Press Ctrl+C to stop the server...\n";
    while (server.IsRunning()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
