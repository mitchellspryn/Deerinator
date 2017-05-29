cmake_minimum_required ( VERSION 2.8 )
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/admin/cmake/Modules/") #Required to find FANN
find_package( OpenCV REQUIRED )
find_package( Threads REQUIRED )

# Set build tree location
set ( CONTROL_BUILD_PATH "${CMAKE_CURRENT_LIST_DIR}/build" )
set ( CONTROL_ADMIN_PATH "${CMAKE_CURRENT_LIST_DIR}/admin" )

message("Current Control Build Path: ${CONTROL_BUILD_PATH}")

# Set up directories to store the built files
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CONTROL_BUILD_PATH}/bin" )
set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CONTROL_BUILD_PATH}/lib" )
#set ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CONTROL_BUILD_PATH}/lib" )

# Default compilation flags
set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall -g -std=c++0x -lpthread -lncurses -Wno-write-strings" )
set ( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -std=c++0x -lpthread -lncurses -Wno-write-strings" )


# Check this directory for additional header files (.hpp/.h)
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/lib" )
find_package("FANN")


# Build all libraries
add_subdirectory( "./lib" "${CONTROL_BUILD_PATH}/lib" )



# Build all utilites, tests, and the final project
add_subdirectory( "./util" "${CONTROL_BUILD_PATH}/util" )
add_subdirectory( "./src" "${CONTROL_BUILD_PATH}/src" )
