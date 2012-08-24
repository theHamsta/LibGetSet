cmake_minimum_required(VERSION 2.6.4)

### PROJECT AND VERSION
project(GetSet)
set(GETSET_VERSION_MAJOR 0)
set(GETSET_VERSION_MINOR 9)
set(GETSET_VERSION_PATCH 1)
set(GETSET_VERSION ${GETSET_VERSION_MAJOR}.${GETSET_VERSION_MINOR}.${GETSET_VERSION_PATCH} )

### EXTERNAL DEPENDENCIES
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

### DIRECTORIES
# INCLUDE_INSTALL_DIR and LIB_INSTALL_DIR must not be absolute paths
set(LIB_INSTALL_DIR_SUFFIX "")
# CACHE STRING "The directories where to install libraries to")
set(LIB_INSTALL_DIR lib${LIB_INSTALL_DIR_SUFFIX})
set(CMAKECONFIG_INSTALL_DIR ${LIB_INSTALL_DIR}) 

set(INCLUDE_INSTALL_DIR "include")
set(BIN_INSTALL_DIR "bin")

### SOURCE CODE
add_subdirectory(GetSet)
add_subdirectory(GetSetGui)
add_subdirectory(Apps)

# Export all targets into the export set "GetSetExport".
install(EXPORT GetSetExport DESTINATION ${CMAKECONFIG_INSTALL_DIR} FILE GetSetTargets.cmake )

# figure out the relative path from the installed Config.cmake file to the install prefix
file(RELATIVE_PATH relInstallDir ${CMAKE_INSTALL_PREFIX}/${CMAKECONFIG_INSTALL_DIR} ${CMAKE_INSTALL_PREFIX} )

# Create GetSetConfig.cmake and GetSetConfigVersion.cmake files for find_package()
configure_file(GetSetConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/GetSetConfig.cmake @ONLY )
configure_file(GetSetConfigVersion.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/GetSetConfigVersion.cmake @ONLY )

# Install these two files into the same directory as the generated exports-file.
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GetSetConfigVersion.cmake ${CMAKE_CURRENT_BINARY_DIR}/GetSetConfigVersion.cmake DESTINATION ${CMAKECONFIG_INSTALL_DIR})

### Installer 2do

# Windows only : copy .dlls to bin folder. FIXME
#if (WIN32)
#    install(FILES ${QT_BINARY_DIR}/QtCore4.dll ${QT_BINARY_DIR}/QtGui4.dll DESTINATION ${BIN_INSTALL_DIR})
#endif (WIN32)
