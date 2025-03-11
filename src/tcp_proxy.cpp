#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define LISTEN_PORT 8080
#define BACKEND_PORT 9000

void client_handling(int client_sock){
    int backend_sock = socket(AF_INET,SOCK_STREAM,0);
    if(backend_sock == -1){
        std::cerr << "Error creating backend socket\n";
        close(client_sock);
        return;
    }

    sockaddr_in backend_addr{};
    backend_addr.sin_family = AF_INET;
    backend_addr.sin_port = htons(BACKEND_PORT);
    inet_pton(AF_INET,"127.0.0.1",&backend_addr.sin_addr);

    if(connect(backend_sock,(struct sockaddr*)&backend_addr, sizeof(backend_addr))==-1){
        std::cerr << "Failed to connect to backend\n";
        close(client_sock);
        close(backend_sock);
        return;
    }


    char buffer[4096];
    ssize_t bytes_received,bytes_sent;

    //forwarding data between client and backedn

     while(true){
        //receiving data from client
        bytes_received = recv(client_sock, buffer, sizeof(buffer),0);
        if(bytes_received<=0) break;//client disconnect

        //forwarding to backend
        bytes_sent = send(backend_sock, buffer, bytes_received,0);
        if(bytes_sent == - 1 ) break; //error sending to backend

        //receive response from backend
        bytes_received = send(backend_sock,buffer, bytes_received,0);
        if(bytes_received<=0) break; //backend closed connection

        //forward to client
        bytes_sent = send(client_sock, buffer,bytes_received,0);
        if(bytes_sent == -1 )break;// eror sending to client

     }

    close(client_sock);
    close(backend_sock);

}


int main(){
    int server_sock = socket(AF_INET,SOCK_STREAM,0);
    if(server_sock== -1){
        std::cerr << "Failed to create server socket\n";
        return 1;
    }
    
    sockaddr_in server_addr{};
    server_addr.sin_family= AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(LISTEN_PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind failed\n";
        close(server_sock);
        return 1;
    }

    if(listen(server_sock, 10 )== -1){
        std::cerr << "Listen failed\n";
        close(server_sock);
        return 1;
    }
    
    std::cout << "Listening on port " << LISTEN_PORT << "...\n";

    while (true){
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_sock = accept(server_sock,(struct sockaddr*)&client_addr,&client_len);

        if (client_sock == -1){
            std::cerr << "Accept failed \n";
            continue;
        }

        std::cout << "New client connected!'\n";
        client_handling(client_sock);

    }

    close(server_sock);
}
