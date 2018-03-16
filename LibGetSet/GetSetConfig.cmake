get_filename_component(myDir ${CMAKE_CURRENT_LIST_FILE} PATH) # get the directory where I myself am
get_filename_component(rootDir ${myDir} ABSOLUTE) # get the chosen install prefix

# set the version of myself
set(GETSET_VERSION_MAJOR 2)
set(GETSET_VERSION_MINOR 7)
set(GETSET_VERSION_PATCH 0)
set(GETSET_VERSION ${GETSET_VERSION_MAJOR}.${GETSET_VERSION_MINOR}.${GETSET_VERSION_PATCH} )

# what is my include directory
set(GETSET_INCLUDE_DIRS "${rootDir}/include")
set(GETSET_DIR "${rootDir}")

# import the exported targets
include(${myDir}/cmake/GetSetTargets.cmake)

# set the expected library variable
set(GETSET_LIBRARIES GetSet GetSetGui )

# Yay!
set(GETSET_FOUND 1)
