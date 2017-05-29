###
# Accelerator Library CMake file
#
# Configures the accelerator library to be built.
#
##

set ( LIBRARY_NAME "accelerator" )

set ( LIBRARY_HEADERS
    "toyAccel.hpp"
  )

set ( LIBRARY_SOURCES
    "toyAccel.cpp"
  )

add_library("${LIBRARY_NAME}" ${LIBRARY_SOURCES} )
target_link_libraries( "${LIBRARY_NAME}" "server")
