cmake_minimum_required(VERSION 3.10)
project(SocketRookie)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED)

# add_executable(demo1 demo1.cpp)
# add_executable(demo_client demo_client.cpp)
# add_executable(demo2 demo2.cpp)
# add_executable(demo_server demo_server.cpp)
# add_executable(tcpselect tcpselect.cpp)
# add_executable(clientselect clientselect.cpp)
# add_executable(demo_epoll1 demo_epoll1.cpp)
# add_executable(client client.cpp)
# 自动查找当前目录下所有cpp文件
file(GLOB SOURCES "*.cpp")

# 为每个cpp文件生成独立可执行文件
foreach(source_file ${SOURCES})
    # 去除扩展名作为可执行文件名（如client.cpp -> client）
    get_filename_component(exec_name ${source_file} NAME_WE)
    add_executable(${exec_name} ${source_file})
endforeach()


