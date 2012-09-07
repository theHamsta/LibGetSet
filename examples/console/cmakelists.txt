cmake_minimum_required(VERSION 2.6.4)

find_package(GETSET REQUIRED)
include_directories(${GETSET_INCLUDES})

add_executable(Simple simple.cpp)
target_link_libraries(Simple GetSet)