#ifndef HEALTH_CHECK_H
#define HEALTH_CHECK_H

#include <string>
#include <atomic>

extern std::atomic<bool> is_ncurses_ui_active;

void health_check(const std::string& ip,int port);
void monitor_backends();


#endif //HEALTH_CHECK_H