#include "../include/health_check.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include "../include/tcp_proxy.h"  


void health_check(const std::string& ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        return;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0) {
        std::cout << "Server " << ip << ":" << port << " is healthy.\n";
    } else {
        perror(("Failed to connect to " + ip + ":" + std::to_string(port)).c_str());
    }

    close(sock);
}



void monitor_backends() {
    std::vector<std::thread> threads;  
    for (const auto& backend : backends) {
        
        threads.push_back(std::thread(health_check, backend.first, backend.second));
    }
    
   
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();  
        }
    }
}
