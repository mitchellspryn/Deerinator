###
# Interface Library CMake file
#
# Configures the Interface library to be built.
#
##

set ( LIBRARY_NAME "interface" )

set ( LIBRARY_HEADERS
    "IAccelerator.hpp"
    "ICamera.hpp"
    "ICommandable.hpp"
    "IDeerTracker.hpp"
    "IGimble.hpp"
    "IServer.hpp"
  )

set ( LIBRARY_SOURCES
    "dummy.cpp"
  )

add_library("${LIBRARY_NAME}" ${LIBRARY_SOURCES} )
target_link_libraries( "${LIBRARY_NAME}" )
