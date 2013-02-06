include_directories("${PROJECT_SOURCE_DIR}")
add_executable(GetSetTest
	test.cpp
	Process.h
	Process.cpp
)
add_dependencies(GetSetTest GetSetGui)
target_link_libraries(GetSetTest GetSet GetSetGui ${QT_LIBRARIES})
