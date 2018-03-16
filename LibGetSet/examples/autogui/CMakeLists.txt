cmake_minimum_required(VERSION 2.6.4)

# Note: This project does not use GetSetGui and therefore does not depend on Qt.

find_package(GETSET REQUIRED)
include_directories(${GETSET_INCLUDE_DIRS})

add_executable(CommandLineApp main.cpp)
target_link_libraries(CommandLineApp GetSet)

install(TARGETS CommandLineApp RUNTIME DESTINATION ${GETSET_DIR}/bin)
