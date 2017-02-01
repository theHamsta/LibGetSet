include_directories("${PROJECT_SOURCE_DIR}")

###
# My own test
###

add_executable(GetSetTest
	test.cpp
)
target_link_libraries(GetSetTest GetSet GetSetGui)
qt5_use_modules(GetSetTest Widgets)

###
# GetSet config tool 
###

add_executable(config
#	AutoGUI/Process.h
#	AutoGUI/Process.cpp
	getsetconfig.cpp
)
target_link_libraries(config GetSet)

###
# GetSet GetSetScript tool 
###

add_executable(GetSetScript
	getsetscript.cpp
)
qt5_use_modules(GetSetScript Core Gui Widgets)
target_link_libraries(GetSetScript GetSet GetSetGui Qt5::Widgets)
install(TARGETS GetSetScript DESTINATION bin)
install(FILES ../Apps/GetSetScript.getset DESTINATION bin)

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
