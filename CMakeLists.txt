cmake_minimum_required(VERSION 3.10)
project(RootProject)  # 根项目名称可自定义

# 全局配置：C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加 muduo 库的路径（确保头文件和库已安装到系统目录）
include_directories(/usr/local/include)     # muduo 头文件路径
link_directories(/usr/local/lib)            # muduo 库路径

# 添加子项目
add_subdirectory(muduotest)     # 包含 muduotest 子项目
add_subdirectory(sockettest)  # 包含 sockettest 子项目