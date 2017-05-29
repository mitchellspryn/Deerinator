###
# AI Library CMake file
#
# Configures the AI library to be built.
#
##

set ( LIBRARY_NAME "AI" )

set ( LIBRARY_HEADERS
    "ArrayStack.hpp"
    "AutonomyDaemon.hpp"
    "ImageDeerTracker.hpp"
    "VideoDeerTracker.hpp"
    "AICamerWrapper.hpp"
    "OCVCapture.h"
    "svm.h"
  )

set ( LIBRARY_SOURCES
    "ArrayStack.cpp"
    "AutonomyDaemon.cpp"
    "ImageDeerTracker.cpp"
    "VideoDeerTracker.cpp"
    "AICameraWrapper.cpp"
    "OCVCapture.cpp"
    "svm.cpp"
  )

add_library("${LIBRARY_NAME}" ${LIBRARY_SOURCES} )
target_link_libraries( "${LIBRARY_NAME}" ${OpenCV_LIBS} ${FANN_LIBRARIES} "m" "v4l2")
