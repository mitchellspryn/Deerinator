# Name: POCO Net Library
# Link: http://pocoproject.org/
# Use:  find_package(PocoNet)
#
# Once done this will define
#  POCONet_FOUND        - System has the Poco Net library
#  POCONet_INCLUDE_DIRS - The include directories
#  POCONet_LIBRARIES    - The libraries needed to use Poco Net


# Dependencies
find_package(POCOFoundation)


# Find the main header file to determine the header directory
find_path(POCONet_INCLUDE_DIR Poco/Net/HTTPServer.h
          /usr/include /usr/local/include)

# Find the library file
find_library(POCONet_LIBRARY PocoNet
             /usr/lib /usr/local/lib)


# Set our "return" variables
set(POCONet_LIBRARIES ${POCONet_LIBRARY} )
set(POCONet_INCLUDE_DIRS ${POCONet_INCLUDE_DIR} )


# Set *_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PocoNet DEFAULT_MSG
                                  POCONet_LIBRARY POCONet_INCLUDE_DIR)


# Hide these internal variables from the CMake viewer
mark_as_advanced(POCONet_INCLUDE_DIR POCONet_LIBRARY )
