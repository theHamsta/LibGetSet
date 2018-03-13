project( EpipolarConsistencyPythonBindings )
cmake_minimum_required( VERSION 3.5 )



aux_source_directory(source SOURCES)

add_subdirectory(pybind11)
pybind11_add_module(epipolarepipolar_consistency SOURCES)

target_include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
	)
