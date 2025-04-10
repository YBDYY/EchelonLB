cmake_minimum_required(VERSION 3.10)
project(TCPProxy)

set(CMAKE_CXX_STANDARD 17)
find_package(yaml-cpp REQUIRED)

find_package(yaml-cpp REQUIRED)

add_executable(tcp_proxy 
    src/main.cpp 
    src/health_check.cpp 
    src/tcp_proxy.cpp  
    src/backend_monitor.cpp
    src/web_ui.cpp
)

target_include_directories(tcp_proxy PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/external/crow/include
    ${CMAKE_SOURCE_DIR}/external/json/include/
    ${CMAKE_SOURCE_DIR}/external/asio/asio/include
)

target_link_libraries(tcp_proxy yaml-cpp)
