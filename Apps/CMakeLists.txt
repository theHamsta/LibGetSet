include_directories("${PROJECT_SOURCE_DIR}")

add_executable(GetSetTest
	test.cpp
)
add_dependencies(GetSetTest GetSet GetSetGui)
target_link_libraries(GetSetTest GetSet GetSetGui ${QT_LIBRARIES})

add_executable(AutoGUI
	AutoGUI.cpp
	Process.h
	Process.cpp
)
add_dependencies(AutoGUI GetSet GetSetGui)
target_link_libraries(AutoGUI GetSet GetSetGui ${QT_LIBRARIES})

add_executable(xmlconfig
	testXML.cpp
)
add_dependencies(xmlconfig GetSet)
target_link_libraries(xmlconfig GetSet)
