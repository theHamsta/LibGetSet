# Note: This project does not use GetSetGui and therefore does not depend on Qt.

cmake_minimum_required(VERSION 2.6.4)

find_package(GETSET REQUIRED)
include_directories(${GETSET_INCLUDE_DIRS})

add_executable(Simple simple.cpp)
target_link_libraries(Simple GetSet)
