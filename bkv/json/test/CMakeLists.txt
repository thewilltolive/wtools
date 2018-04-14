set(LIBS bkv_json   bkv_json_tools)
if(BKV_JSON_YAJL)
  wtool_add_test(bkv_json 01 ${LIBS})
  wtool_add_test(bkv_json 02 ${LIBS})
endif()

