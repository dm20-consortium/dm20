find_path(
  islib_INCLUDE_DIR
  NAMES
    DatagramSocket.h DmUtil.h DmManager.h ResultSet.h Tuple.h
  PATHS
    /usr/local/include/is
  PATH_SUFFIXES
)
find_path(
  geos_INCLUDE_DIR
  NAMES
    geos_c.h export.h
  PATHS
    /usr/include
  PATH_SUFFIXES
)


message(STATUS "islib include dirs: ${islib_INCLUDE_DIR}")
if(${islib_INCLUDE_DIR} STREQUAL "islib_INCLUDE_DIR-NOTFOUND")
  set(islib_FOUND "NO")
  return()
endif()
message(STATUS "geos include dirs: ${geos_INCLUDE_DIR}")
if(${geos_INCLUDE_DIR} STREQUAL "geos_INCLUDE_DIR-NOTFOUND")
  set(islib_FOUND "NO")
  return()
endif()
LIST(APPEND islib_INCLUDE_DIR "${geos_INCLUDE_DIR}")


set(islib_LIBRARIES)
set(SEARCH_TARGETS dmclient;tuple;crypto;ssl;z;zstd;geos;geos_c;log4cxx)
foreach(search_target IN LISTS SEARCH_TARGETS)
  find_library(
    ${search_target}_LIB
    NAMES
      ${search_target}
    PATHS
      /usr/local/lib
      /usr/lib
      /usr/lib/x86_64-linux-gnu
  )
  set(n "${search_target}_LIB")
  if("${${n}}" STREQUAL "${search_target}_LIB-NOTFOUND")
    message(STATUS "${n} is not found")
    set(islib_FOUND "NO")
    return()
  endif()
  LIST(APPEND islib_LIBRARIES "${${n}}")
endforeach()

message(STATUS "islib libraries: ${islib_LIBRARIES}")


set(islib_FOUND "YES")
