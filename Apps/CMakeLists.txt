include_directories("${PROJECT_SOURCE_DIR}")

###
# My own test
###

add_executable(GetSetTest
	test.cpp
)
add_dependencies(GetSetTest GetSet GetSetGui)
target_link_libraries(GetSetTest GetSet GetSetGui ${QT_LIBRARIES})

###
# AutoGUI
###

add_executable(AutoGUI
	AutoGUI.cpp
	Process.h
	Process.cpp
	ConfigureProcess.h
)
target_link_libraries(AutoGUI GetSet GetSetGui ${QT_LIBRARIES})
add_dependencies(AutoGUI GetSet GetSetGui)

add_executable(ExampleAutoGUI
	../examples/autogui/main.cpp
)
target_link_libraries(ExampleAutoGUI GetSet)
add_dependencies(ExampleAutoGUI GetSet)

install(TARGETS AutoGUI ExampleAutoGUI RUNTIME DESTINATION ${BIN_INSTALL_DIR})
install(FILES ../examples/autogui/CMakeLists.txt ../examples/autogui/main.cpp  DESTINATION ${CMAKECONFIG_INSTALL_DIR}examples/autogui)

###
# Examples (2do move)
###

add_executable(ExampleSimple
	../examples/console/simple.cpp
)
target_link_libraries(ExampleSimple GetSet)
add_dependencies(ExampleSimple GetSet)

add_executable(ExampleAdvanced
	../examples/gui/advanced.cpp
)
target_link_libraries(ExampleAdvanced GetSet GetSetGui ${QT_LIBRARIES})
add_dependencies(ExampleAdvanced GetSet GetSetGui)

install(TARGETS ExampleSimple ExampleAdvanced RUNTIME DESTINATION ${BIN_INSTALL_DIR})
