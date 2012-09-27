cmake_minimum_required(VERSION 2.6.4)

### EXTERNAL DEPENDENCIES
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

find_package(GETSET REQUIRED)
include_directories(${GETSET_INCLUDES})

add_executable(SimpleGUI simplegui.cpp)
target_link_libraries(SimpleGUI GetSet GetSetGui ${QT_LIBRARIES})

add_executable(AdvancedGUI advanced.cpp)
target_link_libraries(AdvancedGUI GetSet GetSetGui ${QT_LIBRARIES})
