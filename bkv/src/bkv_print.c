#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include "bkv_print.h"
#include "bkv_print_p.h"
#include "bkv_val.h"

static bkv_parse_retval_t
bkv_print_map_open(void *p_data, uint8_t *p_ptr, bkv_key_t key){
    (void)p_data;
    (void)p_ptr;
    printf("BKV : MAP OPEN %d\n",key);
    return(BKV_PARSE_ACTION_NONE);
}

static bkv_parse_retval_t
bkv_print_map_close(void *p_data, bkv_key_t key){
    (void)p_data;
    printf("BKV : MAP CLOSE %d\n",key);

    return(BKV_PARSE_ACTION_NONE);
}


static bkv_parse_retval_t
bkv_print_uint16(void *p_data, uint8_t *p_ptr, int deep_offset, bkv_key_t key, uint16_t value){
    (void)p_data;
    (void)p_ptr;
    (void)deep_offset;
    printf("BKV : UINT16 %d:%d\n",key,value);

    return(BKV_PARSE_ACTION_NONE);
}

static bkv_parse_retval_t
bkv_print_uint32(void *p_data, uint8_t *p_ptr, int deep_offset, bkv_key_t key, uint32_t value){
    (void)p_data;
    (void)p_ptr;
    (void)deep_offset;
    printf("BKV : UINT32 %d:%d\n",key,value);

    return(BKV_PARSE_ACTION_NONE);
}

static bkv_parse_retval_t
bkv_print_uint64(void *p_data, uint8_t *p_ptr, int deep_offset, bkv_key_t key, uint64_t value){
    (void)p_data;
    (void)p_ptr;
    (void)deep_offset;
    printf("BKV : UINT64 %d:%"PRIi64"\n",key,value);

    return(BKV_PARSE_ACTION_NONE);
}

static bkv_parse_retval_t
bkv_print_str(void *p_data, uint8_t *p_ptr, bkv_key_t key, uint8_t *p_str, int len){
    (void)p_data;
    (void)p_ptr;
    printf("BKV : STR %d:%.*s\n",key,len,p_str);

    return(BKV_PARSE_ACTION_NONE);
}

static bkv_parse_retval_t
bkv_print_array_open(void *p_data, uint8_t *p_ptr, int array_len,bkv_key_t key){
    (void)p_data;
    (void)p_ptr;
    (void)array_len;
    printf("BKV : ARRAY OPEN %d\n",key);

    return(BKV_PARSE_ACTION_NONE);
}

static bkv_parse_retval_t
bkv_print_array_close(void *p_data){
    (void)p_data;
    printf("BKV : ARRAY CLOSE\n");

    return(BKV_PARSE_ACTION_NONE);
}

static bkv_parse_retval_t
bkv_print_float(void *p_data, uint8_t *p_ptr, bkv_key_t key, float f){
    (void)p_ptr;
    (void)p_data;
    printf("BKV : FLOAT : %d:%f\n",key,f);

    return(BKV_PARSE_ACTION_NONE);
}

#define BKV_PRINT_FCT(type,TYPE,format)\
    static bkv_parse_retval_t \
bkv_print_##type(void *p_data, uint8_t *p_ptr, bkv_key_t key, type f){\
    (void)p_ptr;\
    (void)p_data;\
    printf("BKV : "#TYPE" : %d:"#format"\n",key,f);\
    return(BKV_PARSE_ACTION_NONE);}

#define BKV_PRINT_FCT_NAME(type) bkv_print_##type

BKV_PRINT_FCT(bool,BOOL,%d)



int bkv_print2(bkv_t h){
    int         l_ret,l_len;
    uint8_t    *l_ptr;
    bkv_val_t   l_value;

    bkv_val_callbacks_t s_bkv_callbacks ={
        bkv_print_map_open,
        bkv_print_map_close,
        bkv_print_array_open,
        bkv_print_array_close,
        bkv_print_uint16,
        bkv_print_uint32,
        bkv_print_uint64,
        bkv_print_str,
        bkv_print_float,
        NULL,
        BKV_PRINT_FCT_NAME(bool)
    };

    bkv_get_head(h,&l_ptr,&l_len);
    bkv_val_init(&l_value,l_ptr);
    if (BKV_OK != (l_ret=bkv_val_foreach(&l_value,&s_bkv_callbacks,NULL))){
    }
    return(l_ret);
}

