# Name: URG Helper C Library
# Link: http://urgwidget.sourceforge.net/
# Use:  find_package(URGC)
#
# Once done this will define
#  URGC_FOUND        - System has the URG Helper library
#  URGC_INCLUDE_DIRS - The include directories
#  URGC_LIBRARIES    - The libraries needed to use URG Helper


# Find the main header file to determine the header directory
find_path(URGC_INCLUDE_DIR urg_c/urg_sensor.h
          /usr/include /usr/local/include)

# Find the library file
find_library(URGC_LIBRARY urg_c
             /usr/lib /usr/local/lib)


# Set our "return" variables
set(URGC_LIBRARIES ${URGC_LIBRARY} )
set(URGC_INCLUDE_DIRS ${URGC_INCLUDE_DIR} )


# Set *_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(URGC DEFAULT_MSG
                                  URGC_LIBRARY URGC_INCLUDE_DIR)


# Hide these internal variables from the CMake viewer
mark_as_advanced(URGC_INCLUDE_DIR URGC_LIBRARY )
