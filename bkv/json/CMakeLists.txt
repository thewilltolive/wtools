set(BKV_JSON_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(src)

if(BUILD_TEST)
  include_directories(${BKV_INC} src)
  add_subdirectory(tools)
  add_subdirectory(test)
endif()
