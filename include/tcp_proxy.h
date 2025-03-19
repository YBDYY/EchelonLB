#ifndef TCP_PROXY_H
#define TCP_PROXY_H

#define LISTEN_PORT 8080
#define BACKEND_PORT 9000

#include <string>
#include <vector>
#include <mutex>
#include <sys/socket.h>

extern std::vector<std::pair<std::string, int>> backends;
extern std::mutex backend_mutex;
extern int current_backend;

void client_handling(int client_sock);
std::pair<std::string, int > get_next_backend();
void load_backends(const std::string &config_file);
int connect_with_retries(int sock, struct sockaddr *addr, socklen_t addrlen);


#endif // TCP_PROXY_H