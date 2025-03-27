cmake_minimum_required(VERSION 3.10)
project(muduotest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED)

add_executable(muduousetest muduousetest.cpp)

target_link_libraries(muduousetest muduo_net muduo_base pthread)
