find_path(
  _dm2proto_INCLUDE_DIR
  NAMES
    object_info_0_8_0.pb.h
    object_info_0_8_1.pb.h
    object_info_0_6_0.pb.h
    freespace_info_0_8_0.pb.h
    freespace_info_0_8_1.pb.h
    freespace_info_0_6_0.pb.h
    sensor_info_0_8_0.pb.h
    signal_info_0_6_0.pb.h
  PATHS
    /usr/local/include/is/proto
  PATH_SUFFIXES
)

if(${_dm2proto_INCLUDE_DIR} STREQUAL "_dm2proto_INCLUDE_DIR-NOTFOUND")
  message(WARNING "failed to find dm2proto include dir")
  set(dm2proto_FOUND "NO")
  return()
endif()

find_path(
  _proto_INCLUDE_DIR
  NAMES
    google/protobuf/port_def.inc
  PATHS
    /usr/local/include
  PATH_SUFFIXES
)

message(STATUS "_proto include dirs: ${_proto_INCLUDE_DIR}")
if(${_proto_INCLUDE_DIR} STREQUAL "_proto_INCLUDE_DIR-NOTFOUND")
  message(WARNING "failed to find proto include dir")
  set(dm2proto_FOUND "NO")
  return()
endif()
set(dm2proto_INCLUDE_DIR "${_dm2proto_INCLUDE_DIR};${_proto_INCLUDE_DIR}")

function(SearchLibraries SEARCH_TARGETS SEARCH_PATHS OUTPUT_VARIABLE)
  foreach(search_target IN LISTS SEARCH_TARGETS)
    find_library(
      ${search_target}_LIB
      NAMES
        ${search_target}
      PATHS
        ${SEARCH_PATHS}
      NO_DEFAULT_PATH
    )
    set(n "${search_target}_LIB")
    if("${${n}}" STREQUAL "${search_target}_LIB-NOTFOUND")
      message(STATUS "${n} is not found")
      set(dm2proto_FOUND "NO")
      return()
    endif()
    LIST(APPEND found_libs "${${n}}")
  endforeach()
  set(${OUTPUT_VARIABLE} ${found_libs} PARENT_SCOPE)
endfunction()

# cmakeするときも，自前でビルドしたprotobufライブラリを見てほしいので，libprotobuf.so等を得るようにした
# 致し方なし
SearchLibraries("dm2proto_api;dm2proto_is" "/usr/local/lib" _dm2proto_LIBRARIES)
SearchLibraries("protobuf;protobuf-lite" "/usr/local/lib" _proto_LIBRARIES)

set(dm2proto_LIBRARIES "${_dm2proto_LIBRARIES};${_proto_LIBRARIES}")
message(STATUS "dm2proto libraries: ${dm2proto_LIBRARIES};")


set(dm2proto_FOUND "YES")
