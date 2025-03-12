#include "../include/tcp_proxy.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <mutex>
#include <vector>


std::vector<std::pair<std::string, int>> backends ={
    {"127.0.0.1",9000},
    {"127.0.0.1",9001},
    {"127.0.0.1",9002}
};

std::mutex backend_mutex;  
int current_backend =0;

//RR for next backend
std::pair<std::string,int> get_next_backend(){
    std::lock_guard<std::mutex>lock(backend_mutex);
    std::pair<std::string,int> selected_backend = backends[current_backend];

    current_backend=(current_backend +1) % backends.size();

    return selected_backend;
    }

void client_handling(int client_sock){

    auto[backend_ip,backend_port] = get_next_backend();
    int backend_sock = socket(AF_INET,SOCK_STREAM,0);
    if(backend_sock == -1){
        std::cerr << "Error creating backend socket\n";
        close(client_sock);
        return;
    }

    sockaddr_in backend_addr{};
    backend_addr.sin_family = AF_INET;
    backend_addr.sin_port = htons(backend_port);
    inet_pton(AF_INET,backend_ip.c_str(),&backend_addr.sin_addr);

    if(connect(backend_sock,(struct sockaddr*)&backend_addr, sizeof(backend_addr))==-1){
        std::cerr << "Failed to connect to backend\n";
        close(client_sock);
        close(backend_sock);
        return;
    }

    struct timeval timeout{};
    timeout.tv_sec = 60;
    timeout.tv_usec = 0;
    setsockopt(client_sock,SOL_SOCKET,SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(client_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    setsockopt(backend_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(backend_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    char buffer[4096];
    ssize_t bytes_received,bytes_sent;

    //forwarding data between client and backedn

     while(true){
        //receiving data from client
        bytes_received = recv(client_sock, buffer, sizeof(buffer),0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                std::cerr << "Client disconnected.\n";
            } else {
                std::cerr << "Timeout: No data received from client in 60 seconds. Closing connection.\n";
            }
            break;
        }


        //forwarding to backend
        ssize_t total_sent = 0;
        while (total_sent < bytes_received) {
            bytes_sent = send(backend_sock, buffer + total_sent, bytes_received - total_sent, 0);
            if (bytes_sent == -1) {
                std::cerr << "Error sending data to backend\n";
                break;
            }
            total_sent += bytes_sent;
        }

        //receive response from backend
        bytes_received = recv(backend_sock, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {  
            if (bytes_received == 0) {
                std::cerr << "Backend closed connection.\n";
    } else {
        std::cerr << "Timeout: No response from the backend in 60 seconds. Closing connection.\n";
    }
    break;
}
     

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



