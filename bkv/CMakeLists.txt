
set(BKV_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BKV_INC ${BKV_BASE_DIR}/inc)
add_subdirectory(src)

if(BUILD_TEST)
  include_directories(src)
  add_subdirectory(test)
endif()

if(BKV_JSON_YAJL)
  add_subdirectory(json)
endif()


