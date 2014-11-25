cmake_minimum_required(VERSION 2.6.4)

### PROJECT AND VERSION
project(GetSet)
set(GETSET_VERSION_MAJOR 2)
set(GETSET_VERSION_MINOR 0)
set(GETSET_VERSION_PATCH 0)
set(GETSET_VERSION ${GETSET_VERSION_MAJOR}.${GETSET_VERSION_MINOR}.${GETSET_VERSION_PATCH} )

### I'd rather use current standards
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
    add_definitions(-mconsole)
endif()

### EXTERNAL DEPENDENCIES
cmake_policy(SET CMP0020 NEW)
find_package(Qt5Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

### Distinguish debug/realse binaries
set(CMAKE_DEBUG_POSTFIX "d")

### DIRECTORIES
set(LIB_INSTALL_DIR "lib/")
set(CMAKECONFIG_INSTALL_DIR "./")
set(INCLUDE_INSTALL_DIR "include/")
set(BIN_INSTALL_DIR "bin/")

# All binaries go into the top level, not their respective subfolders
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

### SOURCE CODE
add_subdirectory(GetSet)
add_subdirectory(GetSetGui)
add_subdirectory(Apps)

# Export all targets into the export set "GetSetExport".
install(EXPORT GetSetExport DESTINATION ${CMAKECONFIG_INSTALL_DIR} FILE GetSetTargets.cmake )

# Create GetSetConfig.cmake and GetSetConfigVersion.cmake files for find_package()
configure_file(GetSetConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/GetSetConfig.cmake @ONLY )
configure_file(GetSetConfigVersion.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/GetSetConfigVersion.cmake @ONLY )

# Install these two files into the same directory as the generated exports-file.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GetSetConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/GetSetConfigVersion.cmake DESTINATION ${CMAKECONFIG_INSTALL_DIR})
# Add license
install(FILES License.txt DESTINATION ${CMAKECONFIG_INSTALL_DIR})

# Windows only : copy .dlls to bin folder.
if (WIN32)
#    install(FILES icudt52.dll icuin52.dll icuuc52.dll DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Release)
	get_target_property(QtCore_DLL Qt5::Core LOCATION)
	get_target_property(QtGui_DLL Qt5::Gui LOCATION)
	get_target_property(QtWidgets_DLL Qt5::Widgets LOCATION)
    install(FILES ${QtCore_DLL} ${QtGui_DLL} ${QtWidgets_DLL} DESTINATION ${BIN_INSTALL_DIR} CONFIGURATIONS Release)
endif (WIN32)

# Copy example source code (Ships as a part of the libraries, and therefore belongs here)
install(FILES ./examples/README.txt DESTINATION ${CMAKECONFIG_INSTALL_DIR}/examples/)
install(FILES ./examples/console/CMakeLists.txt ./examples/console/simple.cpp DESTINATION ${CMAKECONFIG_INSTALL_DIR}/examples/console)
install(FILES ./examples/gui/README.txt ./examples/gui/CMakeLists.txt ./examples/gui/advanced.cpp DESTINATION ${CMAKECONFIG_INSTALL_DIR}/examples/gui)
