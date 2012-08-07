# - Config file for the GetSet package
# It defines the following variables
#  GETSET_INCLUDE_DIRS - include directories for GetSet and GetSetGui
#  GETSET_LIBRARIES    - GetSet and GetSetGui libraries
#  GETSET_EXECUTABLE   - the GetSetCLI executable

# Compute paths
get_filename_component(GETSET_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(GETSET_INCLUDE_DIRS "${GETSET_CMAKE_DIR}/GetSet" "${GETSET_CMAKE_DIR}/GetSetGui" "${GETSET_CMAKE_DIR}/")

# Our library dependencies (contains definitions for IMPORTED targets)
include("${GETSET_CMAKE_DIR}/GetSetDepends.cmake")

# These are IMPORTED targets created by GetSetLibraryDepends.cmake
set(GETSET_LIBRARIES GetSet GetSetGui)
set(GETSET_EXECUTABLE GetSetCLI)
