
#include <stdio.h>
#include <stdarg.h>
#include "bkv_print.h"
#include "bkv_print_p.h"
#include "bkv_val.h"

int bkv_print(bkv_error_st_t st,
              const char *p_function, 
              int line, 
              char *p_fmt, ...){
    int         l_ret;
    va_list     l_va;
    const char *l_st_str=NULL;

    switch(st){
    case BKV_ERROR_ST_DEBUG:
        l_st_str="DEBUG";
        break;
    case BKV_ERROR_ST_INFO:
        l_st_str="INFO";
        break;
    case BKV_ERROR_ST_WARN:
        l_st_str="WARN";
        break;
    case BKV_ERROR_ST_ERROR:
        l_st_str="ERROR";
        break;
    case BKV_ERROR_ST_CRIT:
        l_st_str="CRIT";
        break;
    default:
        l_st_str=NULL;
        break;
    }
    printf("%s:%s:%d:> ",l_st_str,p_function,line);
    va_start(l_va,p_fmt);
    l_ret=vprintf(p_fmt,l_va);
    printf("\n");
    va_end(l_va);
    return(l_ret);
}

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
bkv_print_uint16(void *p_data, uint8_t *p_ptr, bkv_key_t key, uint16_t value){
    (void)p_data;
    (void)p_ptr;
    printf("BKV : UINT16 %d:%d\n",key,value);

    return(BKV_PARSE_ACTION_NONE);
}

static bkv_parse_retval_t
bkv_print_uint32(void *p_data, uint8_t *p_ptr, bkv_key_t key, uint32_t value){
    (void)p_data;
    (void)p_ptr;
    printf("BKV : UINT32 %d:%d\n",key,value);

    return(BKV_PARSE_ACTION_NONE);
}

static bkv_parse_retval_t
bkv_print_uint64(void *p_data, uint8_t *p_ptr, bkv_key_t key, uint64_t value){
    (void)p_data;
    (void)p_ptr;
    printf("BKV : UINT64 %d:%lld\n",key,value);

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
        NULL
    };

    bkv_get_head(h,&l_ptr,&l_len);
    bkv_val_init(&l_value,l_ptr);
    if (BKV_OK != (l_ret=bkv_val_foreach(&l_value,&s_bkv_callbacks,NULL))){
    }
    return(l_ret);
}

