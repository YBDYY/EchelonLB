#include "../include/backend_monitor.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

std::vector<BackendStatus> backend_statuses;
std::mutex status_mutex;
bool is_running = true;

bool check_backend_status(const std::string& ip, int port){
    return(rand()%2)==0;
}

double get_backend_response_time(const std::string& ip, int port){
    return check_backend_status(ip,port) ?(rand()%100)/100.0 : -1.0;
}


void update_backend_statuses() {
    std::vector<std::string> servers = {
        "127.0.0.1:9000",
        "127.0.0.1:9001",
        "127.0.0.1:9002"
    };

    while (is_running) {
        {
            std::lock_guard<std::mutex> lock(status_mutex);
            backend_statuses.clear();

            for (const auto& ip_port : servers) {
                size_t colon_pos = ip_port.find(":");
                std::string ip = ip_port.substr(0, colon_pos);
                int port = std::stoi(ip_port.substr(colon_pos + 1));

                bool is_healthy = check_backend_status(ip, port);
                double response_time = get_backend_response_time(ip, port);

                backend_statuses.push_back({ ip_port, is_healthy, response_time });
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));  
    }
}


void print_backend_statuses() {
    std::lock_guard<std::mutex> lock(status_mutex);

    std::cout << "\nFinal Backend Status:\n";
    for (size_t i = 0; i < backend_statuses.size(); i++) {
        const auto& backend = backend_statuses[i];
        std::cout << "Backend " << (i + 1) << ": " << backend.ip_port
                  << " - " << (backend.is_healthy ? "Healthy" : "Unreachable")
                  << " (" << (backend.response_time < 0 ? "N/A" : std::to_string(backend.response_time) + " s") << ")\n";
    }
}