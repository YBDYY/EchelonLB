#include "../include/ncurses_ui.h"
#include "../include/tcp_proxy.h"
#include <ncurses.h>
#include <vector>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>

bool check_backend_status(const std::string& ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return false;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(sock);
        return false;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return false;
    }

    close(sock);
    return true;
}

extern bool is_ncurses_ui_active;

void start_ncurses_ui() {
    is_ncurses_ui_active = true;
    
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  
    init_pair(2, COLOR_RED, COLOR_BLACK);     

    int row = 2;
    std::vector<std::string> servers = {
        "127.0.0.1:9000",
        "127.0.0.1:9001",
        "127.0.0.1:9002"
    };

    while (true) {
        clear();
        mvprintw(0, 2, "EchelonLB - Load Balancer monitor");
        mvprintw(1, 2, "Press 'q' to quit");

        for (size_t i = 0; i < servers.size(); i++) {
            std::string ip_port = servers[i];
            size_t colon_pos = ip_port.find(":");
            std::string ip = ip_port.substr(0, colon_pos);
            int port = std::stoi(ip_port.substr(colon_pos + 1));

            bool is_healthy = check_backend_status(ip, port);
            std::string status = is_healthy ? "Healthy" : "Unreachable";

           
            if (is_healthy) {
                attron(COLOR_PAIR(1));  
            } else {
                attron(COLOR_PAIR(2));  
            }

            mvprintw(row + i, 4, "Backend %d: %s - %s", (int)(i + 1), ip_port.c_str(), status.c_str());

            
            attroff(COLOR_PAIR(1));  
            attroff(COLOR_PAIR(2));  
        }

        refresh();
        sleep(2);

        int ch = getch();
        if (ch == 'q') break;
    }

    endwin();
    is_ncurses_ui_active = false;
}
