set(BKV_JSON_INC ${BKV_JSON_BASE_DIR}/inc)
set(BKV_JSON_LIB bkv_json)

set(SRCS)

include_directories(${BKV_INC} ${BKV_JSON_INC})
if (BKV_CACHED_DICO)
  add_definitions(-DBKV_CACHED_DICO)
endif()
if (BKV_DIRECT_DICO)
  add_definitions(-DBKV_DIRECT_DICO)
endif()
if (BKV_JSON_YAJL)
  set(SRCS ${SRCS} bkv_json_yajl.c)
  set(LIBS ${LIBS} yajl)
endif()
if (BKV_JSON_SIMPLE)
  set(SRCS ${SRCS} bkv_json_simple.c)
endif()
add_library(${BKV_JSON_LIB} SHARED 
	bkv_json.c
	${SRCS})

target_link_libraries(${BKV_JSON_LIB} bkv bkv_dico ${LIBS})

target_include_directories(${BKV_JSON_LIB} PUBLIC ${BKV_JSON_INC})

