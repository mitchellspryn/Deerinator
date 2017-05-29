set( UTILITY_NAME "Gimble_Test" )
project( "${UTILITY_NAME}" )
add_executable( "${UTILITY_NAME}" "Gimble_Test.cpp" )
add_dependencies( "${UTILITY_NAME}" "interface" "gimble" )
target_link_libraries( "${UTILITY_NAME}" "interface" "gimble")

set( UTILITY_NAME "Gimble_2Test" )
project( "${UTILITY_NAME}" )
add_executable( "${UTILITY_NAME}" "Gimble_2Test.cpp" )
add_dependencies( "${UTILITY_NAME}" "interface" "gimble" )
target_link_libraries( "${UTILITY_NAME}" "interface" "gimble")

set( UTILITY_NAME "Gimble_Client" )
project( "${UTILITY_NAME}" )
add_executable( "${UTILITY_NAME}" "Gimble_Client.cpp" )
add_dependencies( "${UTILITY_NAME}" "interface" "gimble" )
target_link_libraries( "${UTILITY_NAME}" "interface" "gimble")


