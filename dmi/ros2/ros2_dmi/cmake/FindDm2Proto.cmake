find_path(
  _dm2proto_INCLUDE_DIR
  NAMES
    dm2is.pb.h
    object_info_0_8_0.pb.h
    object_info_0_8_1.pb.h
    object_info_0_6_0.pb.h
    freespace_info_0_8_0.pb.h
    freespace_info_0_8_1.pb.h
    freespace_info_0_6_0.pb.h
    sensor_info_0_8_0.pb.h
    signal_info_0_6_0.pb.h
  PATHS
    /usr/local/include/is
  PATH_SUFFIXES
)

if(${_dm2proto_INCLUDE_DIR} STREQUAL "_dm2proto_INCLUDE_DIR-NOTFOUND")
  message(WARNING "failed to find dm2proto include dir")
  set(dm2proto_FOUND "NO")
  return()
endif()

if(DM2_PROTO_PATH)
  set(DM2_PROTO_INCLUDE_PATHS
    "${DM2_PROTO_PATH}/include"
    "/usr/local/include"
    "/usr/include"
  )

  set(DM2_PROTO_LIBRARY_PATHS
    "${DM2_PROTO_PATH}/lib"
    "/usr/local/lib"
    "/usr/lib/x86_64-linux-gnu"
  )
else()
  set(DM2_PROTO_INCLUDE_PATHS
    "/usr/local/include"
    "/usr/include"
  )

  set(DM2_PROTO_LIBRARY_PATHS
    "/usr/local/lib"
    "/usr/lib/x86_64-linux-gnu"
  )
endif()

find_path(
  _proto_INCLUDE_DIR
  NAMES
    google/protobuf/port_def.inc
  PATHS
    ${DM2_PROTO_INCLUDE_PATHS}
  NO_DEFAULT_PATH
)

message(STATUS "_proto include dirs: ${_proto_INCLUDE_DIR}")
if(${_proto_INCLUDE_DIR} STREQUAL "_proto_INCLUDE_DIR-NOTFOUND")
  message(WARNING "failed to find proto include dir")
  set(dm2proto_FOUND "NO")
  return()
endif()
set(dm2proto_INCLUDE_DIR "${_dm2proto_INCLUDE_DIR};${_proto_INCLUDE_DIR}")

function(SearchLibraries SEARCH_TARGETS SEARCH_PATHS OUTPUT_VARIABLE)
  set(found_libs "")

  foreach(search_target IN LISTS SEARCH_TARGETS)
    find_library(
      ${search_target}_LIB
      NAMES ${search_target}
      PATHS ${SEARCH_PATHS}
      NO_DEFAULT_PATH
    )

    set(n "${search_target}_LIB")

    if("${${n}}" STREQUAL "${search_target}_LIB-NOTFOUND")
      message(STATUS "${n} is not found")
      set(dm2proto_FOUND "NO" PARENT_SCOPE)
      return()
    endif()

    list(APPEND found_libs "${${n}}")
  endforeach()

  set(${OUTPUT_VARIABLE} "${found_libs}" PARENT_SCOPE)
endfunction()

SearchLibraries(
  "dm2proto_api;dm2proto_is"
  "/usr/local/lib"
  _dm2proto_LIBRARIES
)

SearchLibraries(
  "protobuf"
  "${DM2_PROTO_LIBRARY_PATHS}"
  _proto_LIBRARIES
)

set(dm2proto_LIBRARIES "${_dm2proto_LIBRARIES};${_proto_LIBRARIES}")
message(STATUS "dm2proto libraries: ${dm2proto_LIBRARIES};")


set(dm2proto_FOUND "YES")
