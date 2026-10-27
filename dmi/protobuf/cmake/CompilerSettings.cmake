# Detect Ubuntu version
file(STRINGS "/etc/os-release" OS_RELEASE)

foreach(line IN LISTS OS_RELEASE)
  if(line MATCHES "^VERSION_ID=\"?([0-9]+\\.[0-9]+)\"?")
    set(UBUNTU_VERSION "${CMAKE_MATCH_1}")
  endif()
endforeach()

message(STATUS "Ubuntu version: ${UBUNTU_VERSION}")

# Select C++ standard
if(DEFINED CPLUS_VER AND NOT "${CPLUS_VER}" STREQUAL "")
  # Explicitly specified C++ standard takes priority
  if(CPLUS_VER MATCHES "^c\\+\\+([0-9]+)$")
    set(CMAKE_CXX_STANDARD "${CMAKE_MATCH_1}")
  else()
    message(FATAL_ERROR "Invalid CPLUS_VER: ${CPLUS_VER}")
  endif()

elseif(UBUNTU_VERSION VERSION_GREATER_EQUAL "24.04")
  set(CMAKE_CXX_STANDARD 17)

else()
  if(DEFINED ENV{ROS_DISTRO})
    set(ROS_DISTRO "$ENV{ROS_DISTRO}")
  endif()

  message(STATUS "ROS_DISTRO: ${ROS_DISTRO}")

  if(ROS_DISTRO STREQUAL "humble")
    set(CMAKE_CXX_STANDARD 17)
  else()
    set(CMAKE_CXX_STANDARD 14)
  endif()
endif()

message(STATUS "CPLUS_VER: ${CPLUS_VER}")
message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common compile options
add_compile_options(
  -Wall
)