#ifndef TCP_PROXY_H
#define TCP_PROXY_H

#define LISTEN_PORT 8080
#define BACKEND_PORT 9000

#include <string>
#include <vector>


void client_handling(int client_sock);
void load_backends(const std::string &config_file);
int connect_with_retries(int sock, struct sockaddr *addr, socklen_t addrlen);


#endif // TCP_PROXY_H