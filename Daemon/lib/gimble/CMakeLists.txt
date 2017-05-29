###
# Gimble Library CMake file
#
# Configures the Interface library to be built.
#
##

set ( LIBRARY_NAME "gimble" )

set ( LIBRARY_HEADERS
    "BBB_Gimble.hpp"
  )

set ( LIBRARY_SOURCES
    "BBB_Gimble.cpp"
  )

add_library("${LIBRARY_NAME}" ${LIBRARY_SOURCES} )
target_link_libraries( "${LIBRARY_NAME}" "server" "interface")
