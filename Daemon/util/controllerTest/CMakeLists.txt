set( UTILITY_NAME "Controller_Test" )
project( "${UTILITY_NAME}" )
add_executable( "${UTILITY_NAME}" "controllerTest.cpp" )
include_directories("${OpenCV_INCLUDE_DIRS}")
add_dependencies( "${UTILITY_NAME}" "interface" "gimble" "server" "accelerator" )
target_link_libraries( "${UTILITY_NAME}" ${OpenCV_LIBS} "interface" "gimble" "server" "accelerator")

