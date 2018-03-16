cmake_minimum_required(VERSION 2.6.4)

### EXTERNAL DEPENDENCIES
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

find_package(GETSET REQUIRED)
include_directories(${GETSET_INCLUDES})

add_executable(ExampleGUI advanced.cpp)
target_link_libraries(ExampleGUI GetSet GetSetGui ${QT_LIBRARIES})

install(TARGETS ExampleGUI RUNTIME DESTINATION ${GETSET_DIR}/bin)

# Windows only : copy Qt-dlls to bin folder for re-distribution.
if (WIN32)
    install(FILES ${QT_BINARY_DIR}/QtCore4.dll ${QT_BINARY_DIR}/QtGui4.dll DESTINATION ${GETSET_DIR}/bin)
endif (WIN32)
