project( EpipolarConsistencyPythonBindings )
cmake_minimum_required( VERSION 3.5 )

find_package (Eigen3 3.2 REQUIRED NO_MODULE)
 

set(CMAKE_CXX_STANDARD 14)
add_definitions(-std=c++14)
set(PYBIND11_CPP_STANDARD -std=c++14)
aux_source_directory(source SOURCES)

add_subdirectory(pybind11)
pybind11_add_module(_epipolar_consistency ${SOURCES})

target_include_directories( _epipolar_consistency 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/
    PRIVATE ${CMAKE_SOURCE_DIR}/EpipolarConsistency/code
    PRIVATE ${CMAKE_SOURCE_DIR}/EpipolarConsistency/code/HeaderOnly
    PUBLIC ${EIGEN3_INCLUDE_DIR}
	)

target_link_libraries(_epipolar_consistency 
    PUBLIC
   LibProjectiveGeometry
   LibUtilsQt
   gomp
   Eigen3::Eigen
)
if(CUDA_FOUND)
target_link_libraries(_epipolar_consistency 
    PUBLIC
   LibUtilsCuda
   LibEpipolarConsistency
   LibEpipolarConsistencyGui
)
endif()
