get_filename_component(myDir ${CMAKE_CURRENT_LIST_FILE} PATH)
get_filename_component(rootDir ${myDir} ABSOLUTE)

# include directories
set(GETSET_INCLUDE_DIRS "${rootDir}/" "${rootDir}/GetSet" "${rootDir}/GetSetGui")

# import the exported targets
include(${myDir}/GetSetTargets.cmake)

# set the expected library variable
set(GETSET_LIBRARIES GetSet GetSetGui )
