#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../include/tcp_proxy.h" 
#include <yaml-cpp/yaml.h>
#include "../include/health_check.h"
#include <thread>



int main() {

   
    load_backends("../config.yaml");

    
    std::thread health_check_thread(monitor_backends);

    
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        std::cerr << "Failed to create server socket\n";
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(LISTEN_PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind failed\n";
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, 10) == -1) {
        std::cerr << "Listen failed\n";
        close(server_sock);
        return 1;
    }

    std::cout << "Listening on port " << LISTEN_PORT << "...\n";

    
    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);

        if (client_sock == -1) {
            std::cerr << "Accept failed\n";
            continue;
        }

        std::cout << "New client connected!\n";
        client_handling(client_sock);
    }

   
    close(server_sock);

    
    if (health_check_thread.joinable()) {
        health_check_thread.join();
    }

    return 0;
}
