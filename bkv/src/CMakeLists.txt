set(BKV_LIB bkv)

include_directories(${BKV_INC})
add_library(${BKV_LIB} SHARED 
	bkv.c 
	bkv_byteorder.c 
	bkv_file.c
	bkv_mem.c
	bkv_val.c
	bkv_print.c)
target_link_libraries(${BKV_LIB} m lg)
target_include_directories(${BKV_LIB} PUBLIC ${BKV_INC} )

