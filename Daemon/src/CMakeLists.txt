set( EXECUTABLE_NAME "Daemon" )
project( "${EXECUTABLE_NAME}" )
add_executable( "${EXECUTABLE_NAME}" "Daemon.cpp" )
include_directories("${OpenCV_INCLUDE_DIRS}")
add_dependencies( "${EXECUTABLE_NAME}" "interface" "gimble" "server" "accelerator" "AI" )
target_link_libraries( "${EXECUTABLE_NAME}" ${OpenCV_LIBS} "interface" "gimble" "server" "accelerator" "AI")

