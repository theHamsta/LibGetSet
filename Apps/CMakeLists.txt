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
# getset config Tool 
###

add_executable(config
	AutoGUI/Process.h
	AutoGUI/Process.cpp
	getsetconfig.cpp
)
add_dependencies(config GetSet)
target_link_libraries(config GetSet)

###
# AutoGUI
###

# AutoConf
add_executable(AutoConf
	AutoGUI/AutoConf.cpp
)
target_link_libraries(AutoConf GetSet GetSetGui ${QT_LIBRARIES})
add_dependencies(AutoConf GetSet GetSetGui)

# AutoGUI host
QT4_WRAP_CPP(LibAutoGUI_MOC AutoGUI/QClientWindow.h)
add_executable(AutoGUI
	AutoGUI/AutoGUI.cpp
	AutoGUI/Process.h
	AutoGUI/Process.cpp
	AutoGUI/ConfigureProcess.h
	AutoGUI/QClientWindow.h
	AutoGUI/QClientWindow.cpp
	${LibAutoGUI_MOC}
)
target_link_libraries(AutoGUI GetSet GetSetGui ${QT_LIBRARIES})
add_dependencies(AutoGUI GetSet GetSetGui)

# Example client
add_executable(ExampleAutoGUI
	../examples/autogui/main.cpp
)
target_link_libraries(ExampleAutoGUI GetSet)
add_dependencies(ExampleAutoGUI GetSet)

# Example Client using header-only version of GetSet
add_executable(ExampleAutoGUI_header_only
	../examples/autogui_header_only/GetSetAutoGui.hxx
	../examples/autogui_header_only/main.cpp
)

# Install AutoGUI binaries and copy source code of examples, especially header-only version of GetSet
install(TARGETS AutoGUI AutoConf config ExampleAutoGUI ExampleAutoGUI_header_only RUNTIME DESTINATION ${BIN_INSTALL_DIR})
install(FILES ../examples/autogui/CMakeLists.txt ../examples/autogui/main.cpp  DESTINATION ${CMAKECONFIG_INSTALL_DIR}examples/autogui)
install(FILES ../examples/autogui_header_only/GetSetAutoGui.hxx ../examples/autogui_header_only/main.cpp ../examples/autogui_header_only/CMakeLists.txt DESTINATION ${CMAKECONFIG_INSTALL_DIR}examples/autogui_header_only)

###
# Examples (2do move; included in solution to make sure they really compile with the current version)
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

install(TARGETS ExampleAdvanced RUNTIME DESTINATION ${BIN_INSTALL_DIR})
