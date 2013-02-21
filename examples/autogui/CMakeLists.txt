cmake_minimum_required(VERSION 2.6.4)

find_package(GETSET REQUIRED)
include_directories(${GETSET_INCLUDE_DIRS})

add_executable(CommandLineApp main.cpp)
target_link_libraries(CommandLineApp GetSet)
