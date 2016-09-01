include_directories("${PROJECT_SOURCE_DIR}")

###
# My own test
###

add_executable(GetSetTest
	test.cpp
)
target_link_libraries(GetSetTest GetSet GetSetGui Qt5::Widgets)
qt5_use_modules(GetSetTest Widgets)

###
# GetSet config Tool 
###

add_executable(config
	AutoGUI/Process.h
	AutoGUI/Process.cpp
	getsetconfig.cpp
)
target_link_libraries(config GetSet)

###
# GetSet script Tool 
###

add_executable(script
	getsetscript.cpp
)
qt5_use_modules(script Core Gui Widgets)
target_link_libraries(script GetSet GetSetGui Qt5::Widgets)

###
# AutoGUI
###

# AutoConf
add_executable(AutoConf
	AutoGUI/AutoConf.cpp
)
qt5_use_modules(AutoConf Widgets)
target_link_libraries(AutoConf GetSet GetSetGui Qt5::Widgets)

# AutoGUI host
add_executable(AutoGUI
	AutoGUI/AutoGUI.cpp
	AutoGUI/Process.h
	AutoGUI/Process.cpp
	AutoGUI/ConfigureProcess.h
#	${LibAutoGUI_MOC}
)
qt5_use_modules(AutoGUI Core Gui Widgets)
target_link_libraries(AutoGUI GetSet GetSetGui Qt5::Widgets)

# Example client
add_executable(ExampleAutoGUI
	../examples/autogui/main.cpp
)
target_link_libraries(ExampleAutoGUI GetSet)

# Example Client using header-only version of GetSet
add_executable(ExampleAutoGUI_header_only
	../examples/autogui_header_only/GetSetAutoGui.hxx
	../examples/autogui_header_only/main.cpp
)

# Install AutoGUI binaries and copy source code of examples, especially header-only version of GetSet
install(TARGETS AutoGUI AutoConf config ExampleAutoGUI ExampleAutoGUI_header_only DESTINATION bin)
install(FILES ../examples/autogui/CMakeLists.txt ../examples/autogui/main.cpp  DESTINATION examples/autogui)
install(FILES ../examples/autogui_header_only/GetSetAutoGui.hxx ../examples/autogui_header_only/main.cpp ../examples/autogui_header_only/CMakeLists.txt DESTINATION examples/autogui_header_only)

###
# Examples (2do move; included in solution to make sure they really compile with the current version)
###

add_executable(ExampleSimple
	../examples/console/simple.cpp
)
target_link_libraries(ExampleSimple GetSet)

add_executable(ExampleAdvanced
	../examples/gui/advanced.cpp
)
target_link_libraries(ExampleAdvanced GetSet GetSetGui Qt5::Widgets)

install(TARGETS ExampleAdvanced DESTINATION bin)
