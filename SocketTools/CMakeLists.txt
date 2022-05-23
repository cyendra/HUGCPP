cmake_minimum_required(VERSION 3.22)
project(SocketTools)

set(CMAKE_CXX_STANDARD 14)

add_executable(server toko_socket.h test_server.cpp)
add_executable(client toko_socket.h test_client.cpp)
