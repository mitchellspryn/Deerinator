# Name: Eigen Library
# Link: http://www.phidgets.com/
# Use:  find_package(Eigen)
#
# Once done this will define
#  EIGEN_FOUND        - System has the Eigen library
#  EIGEN_INCLUDE_DIRS - The include directories
#  EIGEN_LIBRARIES    - The libraries needed to use Eigen


# Find the main header file to determine the header directory
find_path(EIGEN_INCLUDE_DIR Eigen/Eigen
          /usr/include/eigen3 /usr/local/include/eigen3)

# Find the library file
set ( EIGEN_LIBRARY "/usr/lib" )

# Set our "return" variables
set( EIGEN_LIBRARIES )
set( EIGEN_INCLUDE_DIRS ${EIGEN_INCLUDE_DIR} )


# Set *_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EIGEN DEFAULT_MSG
                                  EIGEN_LIBRARY EIGEN_INCLUDE_DIR)


# Hide these internal variables from the CMake viewer
mark_as_advanced(EIGEN_INCLUDE_DIR EIGEN_LIBRARY )
