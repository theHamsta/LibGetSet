cmake_minimum_required(VERSION 3.5.1)

### PROJECT AND VERSION
project(GetSet)
set(GETSET_VERSION_MAJOR 2)
set(GETSET_VERSION_MINOR 5)
set(GETSET_VERSION_PATCH 0)
set(GETSET_VERSION ${GETSET_VERSION_MAJOR}.${GETSET_VERSION_MINOR}.${GETSET_VERSION_PATCH} )

# Distinguish debug/realse binaries. And nothing else.
set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)

# I'd rather use current standards
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
    # and a console window under windows
    if (WIN32)
        add_definitions(-mconsole)
    endif()
endif()

# Shall dynamic or static linkage be the default?
option(BUILD_SHARED_LIBS "Build Shared Libraries" ON)

# Let MSVC export all symbols by default and create output in bin dir
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Enable parallel compilation on MSVC
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    message(STATUS "Added parallel build arguments to CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
endif()

### PACKAGES

# Add Qt5
find_package(Qt5Widgets)
find_package(Qt5OpenGL)
set(CMAKE_AUTOMOC ON)

### SOURCE CODE
add_subdirectory(GetSet)
add_subdirectory(GetSetGui)

# Utilities and tests
add_subdirectory(Apps)

# Export all targets into the export set "GetSetExport".
install(EXPORT GetSetExport DESTINATION cmake FILE GetSetTargets.cmake )

# Create GetSetConfig.cmake and GetSetConfigVersion.cmake files for find_package()
configure_file(GetSetConfig.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/GetSetConfig.cmake @ONLY )
configure_file(GetSetConfigVersion.cmake.in ${CMAKE_CURRENT_SOURCE_DIR}/GetSetConfigVersion.cmake @ONLY )
install(FILES GetSetConfig.cmake GetSetConfigVersion.cmake DESTINATION . )

# Add license
install(FILES License.txt DESTINATION . )

# Copy example source code (Ships as a part of the libraries, and therefore belongs here)
install(FILES ./examples/README.txt DESTINATION examples/)
install(FILES ./examples/console/CMakeLists.txt ./examples/console/simple.cpp DESTINATION examples/console)
install(FILES ./examples/gui/README.txt ./examples/gui/CMakeLists.txt ./examples/gui/advanced.cpp DESTINATION examples/gui)
