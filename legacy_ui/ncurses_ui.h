#ifndef NCURSES_UI_H
#define NCURSES_UI_H

#include <vector>
#include <string>
#include <atomic>

extern std::atomic<bool> is_ncurses_ui_active;
extern std::atomic<bool> is_running;

void start_ncurses_ui();
double get_backend_response_time(const std::string& ip,int port);
bool check_backend_status();
void user_input_listener();
void user_interface();


#endif