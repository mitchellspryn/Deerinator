###
# Server Library CMake file
#
# Configures the Server library to be built.
#
# Requires a variable indicating the root directory to copy public header
# files of the library to.
#   CONTROL_INCLUDE_PATH
##

set ( LIBRARY_NAME "server" )

set ( LIBRARY_HEADERS
  "messages.hpp"
  "TCP_Server.hpp"
  "VideoStreamer.hpp"
  "Command.hpp"
  )

set ( LIBRARY_SOURCES
  "TCP_Server.cpp"
  "VideoStreamer.cpp"
  "Command.cpp"
  )

add_library("${LIBRARY_NAME}" ${LIBRARY_SOURCES} )
target_link_libraries( "${LIBRARY_NAME}" )
