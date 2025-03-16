#include "../include/tcp_proxy.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <mutex>
#include <vector>
#include <yaml-cpp/yaml.h>





std::vector<std::pair<std::string, int>> backends;
std::mutex backend_mutex;  
int current_backend =0;

void load_backends(const std::string &config_file) {
    YAML::Node config = YAML::LoadFile(config_file);

    if (!config["backends"]) {
        std::cerr << "Error: No 'backends' field in config.yaml\n";
        return;
    }

    backends.clear();  // Clear existing backends in case of reload
    for (const auto &backend : config["backends"]) {
        std::string ip = backend["ip"].as<std::string>();
        int port = backend["port"].as<int>();
        backends.emplace_back(ip, port);
    }

    if (backends.empty()) {
        std::cerr << "Error: No backends loaded from config.yaml\n";
    } else {
        std::cout << "Loaded " << backends.size() << " backends from config.yaml\n";
    }
}


std::pair<std::string, int> get_next_backend() {
    std::lock_guard<std::mutex> lock(backend_mutex);
    if (backends.empty()) {
        std::cerr << "Error: No backends available\n";
        return {"", -1};
    }

    std::pair<std::string, int> selected_backend = backends[current_backend];
    current_backend = (current_backend + 1) % backends.size();
    return selected_backend;
}

int connect_with_retries(int sock, struct sockaddr *addr, socklen_t addrlen){
    int max_attempts = 10;
    for (int attempt = 1; attempt <= max_attempts; attempt++){
        int ret = connect(sock, addr, addrlen);
        if (ret == 0) {
            std::cout << "[INFO] Connected to backend on attempt " << attempt << "\n";
            return 0;
        }
        std::cerr << "[ERROR] Failed to connect to backend on attempt " << attempt << ": ";
        perror("Connect error");
        sleep(1);
    }
    return -1;
}




void client_handling(int client_sock) {
    auto [backend_ip, backend_port] = get_next_backend();
    int backend_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (backend_sock == -1) {
        std::cerr << "Error creating backend socket\n";
        close(client_sock);
        return;
    }

    sockaddr_in backend_addr{};
    backend_addr.sin_family = AF_INET;
    backend_addr.sin_port = htons(backend_port);
    inet_pton(AF_INET, backend_ip.c_str(), &backend_addr.sin_addr);

    if (connect_with_retries(backend_sock, (struct sockaddr*)&backend_addr, sizeof(backend_addr)) < 0) {
    perror("Failed to connect to backend after multiple attempts");
    close(client_sock);
    close(backend_sock);
    return;
    }

    struct timeval timeout{};
    timeout.tv_sec = 60;
    timeout.tv_usec = 0;
    setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(client_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(backend_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(backend_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    char buffer[4096];
    ssize_t bytes_received, bytes_sent;

    
    while (true) {
        // receiving data from client
        bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                std::cerr << "Client disconnected.\n";
            } else {
                std::cerr << "Timeout or error receiving from client. Closing connection.\n";
            }
            break;
        }

        // forwarding to backend
        ssize_t total_sent = 0;
        while (total_sent < bytes_received) {
            bytes_sent = send(backend_sock, buffer + total_sent, bytes_received - total_sent, 0);
            if (bytes_sent == -1) {
                std::cerr << "Error sending data to backend\n";
                break;
            }
            total_sent += bytes_sent;
        }

        // receiving response from backend
        bytes_received = recv(backend_sock, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {  
            if (bytes_received == 0) {
                std::cerr << "Backend closed connection.\n";
            } else {
                std::cerr << "Timeout or error receiving from backend. Closing connection.\n";
            }
            break;
        }

        // forward response to client
        total_sent = 0;
        while (total_sent < bytes_received) {
            bytes_sent = send(client_sock, buffer + total_sent, bytes_received - total_sent, 0);
            if (bytes_sent == -1) {
                std::cerr << "Error sending data to client\n";
                break;
            }
            total_sent += bytes_sent;
        }
    }

    
    close(client_sock);
    close(backend_sock);
}




