find_path(
  udp_dmi_INCLUDE_DIR
  NAMES
    config.h
  PATHS
    /usr/local/include
  PATH_SUFFIXES
    udp_dmi
)

message(STATUS "udp_dmi include dirs: ${udp_dmi_INCLUDE_DIR}")
if(${udp_dmi_INCLUDE_DIR} STREQUAL "udp_dmi_INCLUDE_DIR-NOTFOUND")
  set(udp_dmi_FOUND "NO")
  return()
endif()

set(udp_dmi_LIBRARIES)
set(SEARCH_TARGETS udp_dmi_receiver_wrapper;udp_dmi_sender_wrapper)
foreach(search_target IN LISTS SEARCH_TARGETS)
  find_library(
    ${search_target}_LIB
    NAMES
      ${search_target}
    PATHS
      /usr/local/lib
      /usr/lib
  )
  set(n "${search_target}_LIB")
  if("${${n}}" STREQUAL "${search_target}_LIB-NOTFOUND")
    message(STATUS "${n} is not found")
    set(udp_dmi_FOUND "NO")
    return()
  endif()
  LIST(APPEND udp_dmi_LIBRARIES "${${n}}")
endforeach()

message(STATUS "udp_dmi libraries: ${udp_dmi_LIBRARIES}")


set(udp_dmi_FOUND "YES")
