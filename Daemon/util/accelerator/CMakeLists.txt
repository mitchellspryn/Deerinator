set( UTILITY_NAME "toyAccel_Test" )
project( "${UTILITY_NAME}" )
add_executable( "${UTILITY_NAME}" "toyAccel.cpp" )
include_directories("${OpenCV_INCLUDE_DIRS}")
add_dependencies( "${UTILITY_NAME}" "accelerator" )
target_link_libraries( "${UTILITY_NAME}" "accelerator")

