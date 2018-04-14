#library builds.
add_definitions(-std=c99 -pedantic)
add_definitions(-Wmissing-declarations)
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=gnu99 -Wmissing-prototypes -Wbad-function-cast -Wshadow -Wfloat-equal")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wswitch-default -Wall -Wextra")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wdeclaration-after-statement")

add_definitions(-DBKV_USE_POSIX_FILE_ACCESS)


if(${CMAKE_BUILD_TYPE})
  if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-g -O0)
  endif()
endif()

#test macro.
include(CTest)
macro(wtool_add_test comp test)
  set(test_name test_${comp}_${test})
  add_executable(${test_name} test_${comp}_uc${test}.c)
  target_link_libraries(${test_name} cunit ${ARGN})
  add_test(NAME ${test_name}
	  WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
	  COMMAND "${CMAKE_BINARY_DIR}/bin/${test_name}")
endmacro()
