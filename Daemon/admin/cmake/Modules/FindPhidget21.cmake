# Name: Phidget Library
# Link: http://www.phidgets.com/
# Use:  find_package(Phidget21)
#
# Once done this will define
#  PHIDGET21_FOUND        - System has the Phidgets library
#  PHIDGET21_INCLUDE_DIRS - The include directories
#  PHIDGET21_LIBRARIES    - The libraries needed to use Phidgets


# Find the main header file to determine the header directory
find_path(PHIDGET21_INCLUDE_DIR phidget21.h
          /usr/include /usr/local/include)

# Find the library file
find_library(PHIDGET21_LIBRARY phidget21
             /usr/lib /usr/local/lib)


# Set our "return" variables
set(PHIDGET21_LIBRARIES ${PHIDGET21_LIBRARY} )
set(PHIDGET21_INCLUDE_DIRS ${PHIDGET21_INCLUDE_DIR} )


# Set *_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PHIDGET21 DEFAULT_MSG
                                  PHIDGET21_LIBRARY PHIDGET21_INCLUDE_DIR)


# Hide these internal variables from the CMake viewer
mark_as_advanced(PHIDGET21_INCLUDE_DIR PHIDGET21_LIBRARY )
