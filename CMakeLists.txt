project( epipolar_consistency )
cmake_minimum_required( VERSION 3.5 )
set(CMAKE_CXX_STANDARD_REQUIRED 14)
add_definitions(-std=c++14)

add_subdirectory(EpipolarConsistency)
add_subdirectory(EpipolarConsistencyPythonBindings)