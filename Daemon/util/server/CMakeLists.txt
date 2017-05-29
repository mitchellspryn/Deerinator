#set( UTILITY_NAME "TCP_Server_Test" )
#project( "${UTILITY_NAME}" )
#add_executable( "${UTILITY_NAME}" "TCP_Server_Test.cpp" )
#add_dependencies( "${UTILITY_NAME}" "interface" "server" )
#target_link_libraries( "${UTILITY_NAME}" "interface" "server" )

set( UTILITY_NAME "VideoStreamer_Test" )
project( "${UTILITY_NAME}" )
add_executable( "${UTILITY_NAME}" "VideoStreamer_Test.cpp" )
add_dependencies( "${UTILITY_NAME}" "interface" "server" )
target_link_libraries( "${UTILITY_NAME}" "interface" "server" )

