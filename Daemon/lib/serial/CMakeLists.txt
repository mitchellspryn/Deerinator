###
# Serial port Library CMake file
#
# Configures the Interface library to be built.
#
##

set ( LIBRARY_NAME "serial" )

set ( LIBRARY_HEADERS
    "serial.hpp"
  )

set ( LIBRARY_SOURCES
    "serial.cpp"
  )

add_library("${LIBRARY_NAME}" ${LIBRARY_SOURCES} )
target_link_libraries( "${LIBRARY_NAME}")
