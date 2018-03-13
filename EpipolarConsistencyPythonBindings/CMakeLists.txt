project( EpipolarConsistencyPythonBindings )
cmake_minimum_required( VERSION 3.5 )

find_package (Eigen3  REQUIRED NO_MODULE)
 

aux_source_directory(source SOURCES)

add_subdirectory(pybind11)
pybind11_add_module(_epipolar_consistency ${SOURCES})

target_include_directories( _epipolar_consistency 
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/
    PRIVATE ${CMAKE_SOURCE_DIR}/EpipolarConsistency/code
    PUBLIC ${EIGEN3_INCLUDE_DIR}
	)

target_link_libraries(_epipolar_consistency 
    PUBLIC
   LibProjectiveGeometry
   LibUtilsCuda
   LibUtilsQt
   LibEpipolarConsistency
   LibEpipolarConsistencyGui
   gomp
    )
