###
# Camera Library CMake file
#
# Configures the Interface library to be built.
#
##

set ( LIBRARY_NAME "camera" )

set ( LIBRARY_HEADERS
    "RealCamera.hpp"
  )

set ( LIBRARY_SOURCES
    "RealCamera.cpp"
  )

add_library("${LIBRARY_NAME}" ${LIBRARY_SOURCES} )
target_link_libraries( "${LIBRARY_NAME}" )
