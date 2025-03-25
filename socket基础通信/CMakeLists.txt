cmake_minimum_required(VERSION 3.10)
project(SocketRookie)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED)

add_executable(demo1 demo1.cpp)
add_executable(demo_client demo_client.cpp)
add_executable(demo2 demo2.cpp)
add_executable(demo_server demo_server.cpp)
add_executable(tcpselect tcpselect.cpp)
add_executable(clientselect clientselect.cpp)
