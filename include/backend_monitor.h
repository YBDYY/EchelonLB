#ifndef BACKEND_MONITOR_H
#define BACKENd_MONITOR_H

#include <vector>
#include <string>
#include <mutex>

struct BackendStatus{
    std::string ip_port;
    bool is_healthy;
    double response_time;
};

extern std::vector<BackendStatus> backend_statuses;
extern std::mutex status_mutex;

void update_backend_statuses();
void print_backend_statuses();



#endif