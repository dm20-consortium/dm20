find_path(
  protobuf_dmi_INCLUDE_DIR
  NAMES
    protobuf_dmi.hpp dm_bridge.hpp socket.hpp uploader.hpp utils.hpp
  PATHS
    /usr/local/include
  PATH_SUFFIXES
    protobuf_dmi
)

message(STATUS "protobuf_dmi include dirs: ${protobuf_dmi_INCLUDE_DIR}")
if(${protobuf_dmi_INCLUDE_DIR} STREQUAL "protobuf_dmi_INCLUDE_DIR-NOTFOUND")
  set(protobuf_dmi_FOUND "NO")
  return()
endif()

set(protobuf_dmi_LIBRARIES)
set(SEARCH_TARGETS protobuf_dmi)
foreach(search_target IN LISTS SEARCH_TARGETS)
  find_library(
    ${search_target}_LIB
    NAMES
      ${search_target}
    PATHS
      /usr/local/lib
  )
  set(n "${search_target}_LIB")
  if("${${n}}" STREQUAL "${search_target}_LIB-NOTFOUND")
    message(STATUS "${n} is not found")
    set(protobuf_dmi_FOUND "NO")
    return()
  endif()
  LIST(APPEND protobuf_dmi_LIBRARIES "${${n}}")
endforeach()

message(STATUS "protobuf_dmi libraries: ${protobuf_dmi_LIBRARIES}")


set(protobuf_dmi_FOUND "YES")
