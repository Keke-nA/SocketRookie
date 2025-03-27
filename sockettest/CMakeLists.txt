project(sockettest)

# 自动查找当前目录下所有cpp文件
file(GLOB SOURCES "*.cpp")
# 为每个cpp文件生成独立可执行文件
foreach(source_file ${SOURCES})
    # 去除扩展名作为可执行文件名（如client.cpp -> client）
    get_filename_component(exec_name ${source_file} NAME_WE)
    add_executable(${exec_name} ${source_file})
endforeach()


