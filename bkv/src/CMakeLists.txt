set(BKV_LIB bkv)

include_directories(${BKV_INC})

if(BKV_FILE_SUPPORT)
  set_source_files_properties(bkv_file.c PROPERTIES COMPILE_FLAGS -D_GNU_SOURCE)
  set(BKV_FILE_SOURCE bkv_file.c)
endif(BKV_FILE_SUPPORT)

add_library(${BKV_LIB} SHARED 
	bkv.c 
	bkv_byteorder.c 
	${BKV_FILE_SOURCE}
	bkv_mem.c
	bkv_val.c
	bkv_print.c)
target_link_libraries(${BKV_LIB} m lg)
target_include_directories(${BKV_LIB} PUBLIC ${BKV_INC} )

