cmake_minimum_required(VERSION 3.22)
project(SocketTools)

set(CMAKE_CXX_STANDARD 14)

add_executable(test_server toko_socket.h test_server.cpp)
add_executable(test_client toko_socket.h test_client.cpp)

add_executable(test_select_server toko_socket.h test_select_server.cpp)

add_executable(select_echo_server toko_socket.h select_echo_server.cpp)

