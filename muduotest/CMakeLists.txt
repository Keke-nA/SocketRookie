project(muduotest)

add_executable(muduousetest muduousetest.cpp)
target_link_libraries(muduousetest muduo_net muduo_base pthread)
