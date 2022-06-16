cmake_minimum_required(VERSION 3.22)
project(SocketTools)

set(CMAKE_CXX_STANDARD 14)


link_directories(/usr/local/lib)
link_directories(/usr/lib)

include_directories(/usr/local/include)
include_directories(/usr/include)


add_executable(test_server toko_socket.h test_server.cpp)
add_executable(test_client toko_socket.h test_client.cpp)

add_executable(test_select_server toko_socket.h test_select_server.cpp)

add_executable(select_echo_server toko_socket.h select_echo_server.cpp)

add_executable(libevent_echo toko_socket.h libevent_echo.cpp)
target_link_libraries(libevent_echo event)


add_executable(hello_world_server toko_socket.h toko_libevent.h hello_world_server.cpp)
target_link_libraries(hello_world_server event)
