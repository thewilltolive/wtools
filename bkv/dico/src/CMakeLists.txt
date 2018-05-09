set(BKV_DICO_LIB bkv_dico)

include_directories(${BKV_DICO_INC})
add_library(${BKV_DICO_LIB} SHARED 
	bkv_dico.c 
	bkv_dico_direct.c
	bkv_dico_cached.c)
target_link_libraries(${BKV_DICO_LIB} kt bkv)
target_include_directories(${BKV_DICO_LIB} PUBLIC ${BKV_DICO_INC} )

