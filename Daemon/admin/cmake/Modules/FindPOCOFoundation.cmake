# Name: POCO Library
# Link: http://pocoproject.org/
# Use:  find_package(PocoFoundation)
#
# Once done this will define
#  POCOFoundation_FOUND        - System has the POCO library
#  POCOFoundation_INCLUDE_DIRS - The include directories
#  POCOFoundation_LIBRARIES    - The libraries needed to use POCO


# Find the main header file to determine the header directory
find_path(POCOFoundation_INCLUDE_DIR Poco/BasicEvent.h
          /usr/include /usr/local/include)

# Find the library file
find_library(POCOFoundation_LIBRARY PocoFoundation
             /usr/lib /usr/local/lib)


# Set our "return" variables
set(POCOFoundation_LIBRARIES ${POCOFoundation_LIBRARY} )
set(POCOFoundation_INCLUDE_DIRS ${POCOFoundation_INCLUDE_DIR} )


# Set *_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PocoFoundation DEFAULT_MSG
                                  POCOFoundation_LIBRARY POCOFoundation_INCLUDE_DIR)


# Hide these internal variables from the CMake viewer
mark_as_advanced(POCOFoundation_INCLUDE_DIR POCOFoundation_LIBRARY )
