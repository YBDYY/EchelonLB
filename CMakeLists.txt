cmake_minimum_required(VERSION 3.10)
project(TCPProxy)

set(CMAKE_CXX_STANDARD 17)

find_package(yaml-cpp REQUIRED)

add_executable(tcp_proxy src/main.cpp src/health_check.cpp src/tcp_proxy.cpp src/ncurses_ui.cpp)
target_link_libraries(tcp_proxy yaml-cpp ncurses)
target_include_directories(tcp_proxy PRIVATE include)
