
cmake_minimum_required(VERSION 2.8)

set (sources main.cpp)

add_definitions("-std=c++11")

add_executable(vulkan-test01 ${sources})

target_link_libraries(vulkan-test01 vulkan)
