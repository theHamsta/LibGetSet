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


# AutoGUI host
QT4_WRAP_CPP(AutoGUI_MOC AutoGUI/QClientWindow.h)
add_executable(AutoGUI
	AutoGUI/AutoGUI.cpp
	AutoGUI/Process.h
	AutoGUI/Process.cpp
	AutoGUI/ConfigureProcess.h
	AutoGUI/QClientWindow.h
	AutoGUI/QClientWindow.cpp
	${AutoGUI_MOC}
)
target_link_libraries(AutoGUI GetSet GetSetGui ${QT_LIBRARIES})
add_dependencies(AutoGUI GetSet GetSetGui)

# Example client
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
