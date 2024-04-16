# Findmosquitto
# -----------
#
# Find the mosquitto library.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``MOSQUITTO_FOUND``
#   System has the mosquitto library.
# ``MOSQUITTO_INCLUDE_DIR``
#   The mosquitto include directory.
# ``MOSQUITTO_LIBRARY``
#   The mosquitto library.
# ``MOSQUITTO_LIBRARIES``
#   All mosquitto libraries.
#
# Hints
# ^^^^^
#
# Set ``MOSQUITTO_ROOT_DIR`` to the root directory of an mosquitto installation.
# Set ``MOSQUITTO_USE_STATIC_LIBS`` to ``TRUE`` to look for static libraries.

# Support preference of static libs by adjusting CMAKE_FIND_LIBRARY_SUFFIXES
if(MOSQUITTO_USE_STATIC_LIBS)
    set(_mosquitto_ORING_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
    set(${CMAKE_FIND_LIBRARY_SUFFIXES} .a)
endif()

find_package(PkgConfig QUIET)
pkg_check_modules(MOSQUITTO QUIET "mosquitto")

include(FindPackageHandleStandardArgs)
find_path(MOSQUITTO_INCLUDE_DIR 
    NAMES 
        mosquitto.h
    )
find_library(MOSQUITTO_LIBRARY 
    NAMES
    mosquitto
    )
find_library(MOSQUITTOPP_LIBRARY
    NAMES
    mosquittopp
    )

find_package_handle_standard_args(mosquitto
    DEFAULT_MSG
    MOSQUITTO_INCLUDE_DIR
    MOSQUITTO_LIBRARY
    MOSQUITTOPP_LIBRARY
    )

mark_as_advanced(
    MOSQUITTO_INCLUDE_DIR
    MOSQUITTO_LIBRARY
    MOSQUITTOPP_LIBRARY
    )

if (MOSQUITTO_FOUND)
    set(MOSQUITTO_INCLUDE_DIRS ${MOSQUITTO_INCLUDE_DIR})
    mark_as_advanced(MOSQUITTO_INCLUDE_DIRS)

    set(MOSQUITTO_LIBRARIES ${MOSQUITTO_LIBRARY})
    mark_as_advanced(MOSQUITTO_LIBRARIES)

    set(MOSQUITTOPP_LIBRARIES ${MOSQUITTOPP_LIBRARY} ${MOSQUITTO_LIBRARY})
    mark_as_advanced(MOSQUITTOPP_LIBRARIES)
endif()

# Restore the original find library ordering
if(MOSQUITTO_USE_STATIC_LIBS)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ${_mosquitto_ORING_CMAKE_FIND_LIBRARY_SUFFIXES})
endif()
