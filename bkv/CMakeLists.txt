
set(BKV_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BKV_INC ${BKV_BASE_DIR}/inc)
set(BKV_DICO_INC ${BKV_BASE_DIR}/dico/inc)
include_directories(inc)
add_subdirectory(src)
add_subdirectory(dico)

if(BUILD_TEST)
  include_directories(src)
  add_subdirectory(test)
endif()

if(BKV_JSON_YAJL)
  add_subdirectory(json)
endif()


