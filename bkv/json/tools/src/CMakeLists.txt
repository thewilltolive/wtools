set(BKV_JSON_TOOLS_INC ${BKV_JSON_TOOLS_BASE_DIR}/inc)
set(BKV_JSON_TOOLS_LIB bkv_json_tools)
message("${BKV_JSON_TOOLS_INC}")
include_directories(${BKV_JSON_TOOLS_INC})


add_library(${BKV_JSON_TOOLS_LIB} SHARED 
	bkv_json_tools.c
	${SRCS})

target_link_libraries(${BKV_JSON_TOOLS_LIB} yajl)

target_include_directories(${BKV_JSON_TOOLS_LIB} PUBLIC ${BKV_JSON_TOOLS_INC})

