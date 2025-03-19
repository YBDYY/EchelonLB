#ifndef NCURSES_UI_H
#define NCURSES_UI_H

#include <vector>
#include <string>

extern bool is_ncurses_ui_active;

void start_ncurses_ui();
double get_backend_response_time(const std::string& ip,int port);


#endif