#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>
#include "bkv.h"
#include "bkv_print_p.h"
#include "lg.h"
#ifdef BKV_USE_POSIX_FILE_ACCESS
#include "bkv_file.h"
#endif
#include "bkv_mem.h"
#include "bkv_header.h"
#include "bkv_byteorder.h"

/* local definitions **********************/
#define BKV_DEFAULT_MEM_CHUNK_SIZE (128)

typedef enum {
    STATE_INIT,
    STATE_IN_MAP,
    STATE_KEY_SET,
    STATE_IN_ARRAY,
    STATE_TERM
} state_t;

typedef enum {
    OPEN_MAP,
    CLOSE_MAP,
    OPEN_ARRAY,
    CLOSE_ARRAY,
    SET_ARRAY_KEY,
    SET_KEY,
    SET_VALUE
} action_t;

struct bkv_s{
    int      fd;
    state_t  state;                  /*!< Current creation state. */
    uint8_t *ptr;                    /*!< base pointer of the bkv, whereas it is memory only or a mmapped file. */
    int      mem_size;               /*!< size of the ptr. */
    int      write_offset;
    int      max_size;               /*!< Maximum size of the created raw data defined when #bkv_create is called.*/
    int      mem_chunk_size;         /*!< size of chunks reallocated when ptr becomes to short. */

    int      deep_offset;
#define MAP_KEY   '{'
#define ARRAY_KEY '['
    char     deep_value[32];         /*!< { means an object. [ means an array. */
    int      map_offset[32];
    int      array_nb_elems[32];

    bkv_t    end_hdl;
};

/* static declarations ********************/
static pthread_mutex_t s_mutex = PTHREAD_MUTEX_INITIALIZER;
static int             s_init  = 0;
int bkv_print_bkv(const char *p_function, int line, bkv_t h);
int bkv_print_state(const char *p_function, int line, bkv_t h);

int bkv_size(bkv_t h){
    return(h->write_offset);
}

int bkv_init(bkv_init_t *i){
    int l_ret=BKV_INV_STATE;
    if (NULL == i){
        return(l_ret);
    }
    pthread_mutex_lock(&s_mutex);
    (void)i;
    if (s_init == 0){
        s_init ++;
        pthread_mutex_unlock(&s_mutex);
        l_ret=BKV_OK;
    }
    else {
        pthread_mutex_unlock(&s_mutex);
    }
    return(l_ret);
}

int bkv_term(void){
    int l_ret=BKV_INV_STATE;
    pthread_mutex_lock(&s_mutex);
    if (s_init > 0){
        s_init=0;
        pthread_mutex_unlock(&s_mutex);
        l_ret=BKV_OK;
    }
    else{
        pthread_mutex_unlock(&s_mutex);
    }
    return(l_ret);
}

static int bkv_check_state(state_t s, char t, action_t a){
    int l_ret=-1;
    BKV_FCT_INIT(bkv_check_state);
    switch(s){
    case STATE_INIT:
        if (a == OPEN_MAP){
            l_ret=0;
        }
        break;
    case STATE_IN_MAP:
        if (((t == MAP_KEY) && 
            ((a == SET_KEY)||(a == CLOSE_MAP)||(a==OPEN_MAP)||(a==OPEN_ARRAY))) ||
            ((t == ARRAY_KEY) &&
             ((a==OPEN_MAP) || (a==OPEN_ARRAY)))){
            l_ret=0;
        }
        break;
    case STATE_KEY_SET:
        break;
    case STATE_IN_ARRAY:
        if ((a == OPEN_MAP) || 
            (a == SET_ARRAY_KEY) || 
            (a == CLOSE_ARRAY)){
            l_ret=0;
        }
        break;
    default:
#ifdef DEBUG
        lg_print(LG_LEVEL_DEBUG,BKV_FCT_NAME,__LINE__,"Invalid state %d\n",s);
#endif
        break;
    }
    return(l_ret);
}

int bkv_create(bkv_create_t *p_create, bkv_t *p_h){
    int          l_fd=-1;
    int          l_size=-1;
    int          l_ret=BKV_OK;
    int          l_default_write_offset=0;
    bkv_t        l_bkv=NULL;
    state_t      l_state=STATE_INIT;
    uint8_t     *l_ptr=NULL;

    assert(NULL != p_create);
    assert(NULL != p_h);

    switch(p_create->create_type){
#ifdef BKV_USE_POSIX_FILE_ACCESS
    case BKV_CREATE_TYPE_OPEN_FILE_CREAT_AND_WRITE:
        l_ret=bkv_open_file_create_write(p_create->filename,p_create->mode,&l_fd,&l_size,&l_ptr);
        break;
    case BKV_CREATE_TYPE_OPEN_FILE_UPDATE:
        l_ret=bkv_open_file_update(p_create->filename,p_create->mode,&l_fd,&l_size,&l_ptr);
        l_state=STATE_TERM;
        break;
    case BKV_CREATE_TYPE_OPEN_FILE_READ_ONLY:
        if (BKV_OK != (l_ret=bkv_open_file_readonly(p_create->filename,&l_fd,&l_size,&l_ptr))){
        }
        else {
            l_state=STATE_TERM;
        }
        break;
#endif
    case BKV_CREATE_TYPE_WORK_IN_RAM:
        if (NULL == (l_ptr=malloc(BKV_DEFAULT_MEM_CHUNK_SIZE))){
            l_ret=BKV_HDL_MEM;
        }
        else {
            l_size=BKV_DEFAULT_MEM_CHUNK_SIZE;
            memset(l_ptr,0,BKV_DEFAULT_MEM_CHUNK_SIZE);
        }
        break;
    case BKV_CREATE_TYPE_SAFE_INPUT_BUFFER:
        l_ptr=p_create->input_ptr;
        break;
    case BKV_CREATE_TYPE_UNSAFE_INPUT_BUFFER:
        if (NULL == (l_ptr=malloc(p_create->input_ptr_length))){
        }
        else {
            memcpy(l_ptr,p_create->input_ptr,p_create->input_ptr_length);
            l_default_write_offset=p_create->input_ptr_length;
            l_size=p_create->input_ptr_length;
            l_ret=BKV_OK;
        }
        break;
    default:
        l_ret=BKV_INV_ARG;
        break;
    }
    if (BKV_OK == l_ret){
        if (NULL == (l_bkv = malloc(sizeof(*l_bkv)))){
            l_ret=BKV_HDL_MEM;
        }
        else {
            memset(l_bkv,0,sizeof(*l_bkv));
            l_bkv->state=l_state;
            l_bkv->fd=l_fd;
            l_bkv->ptr=l_ptr;
            l_bkv->mem_size=l_size;
            l_bkv->max_size=-1;
            l_bkv->write_offset=l_default_write_offset;
            l_bkv->mem_chunk_size=p_create->mem_chunk_size;
            l_bkv->deep_offset=-1;
            memset(&l_bkv->deep_value[0],0,sizeof(l_bkv->deep_value));
            *p_h=l_bkv;
        }
    }
    return(l_ret);
}

int bkv_finalize(bkv_t handle){
    int l_ret=-1;
    if (NULL != handle){
        if (BKV_INVALID != handle->end_hdl){
            bkv_append(handle,handle->end_hdl);
        }
#ifdef BKV_USE_POSIX_FILE_ACCESS
        if (handle->fd != -1){
            l_ret = bkv_file_truncate(handle->fd,handle->mem_size);
        }
#endif
        else if (NULL != handle->ptr){
            free(handle->ptr);
            free(handle);
            l_ret=0;
        }
        else {
            l_ret=0;
        }
    }
    return(l_ret);
}

int bkv_kv_closed(bkv_t   handle,
                  bool   *p_finalized){
    if (NULL == handle){
        return(LG_E_BAD_PARAMETER);
    }
    if (NULL == p_finalized){
        return(LG_E_BAD_PARAMETER);
    }
    *p_finalized=handle->state==STATE_TERM;
    return(BKV_OK);
}

int bkv_destroy(bkv_t handle){
    int l_ret=-1;
    if (NULL != handle){
#ifdef BKV_USE_POSIX_FILE_ACCESS
        if (handle->fd != -1){
            l_ret = bkv_file_close(handle->fd,handle->ptr,handle->mem_size);
        }
        else 
#endif
            if (NULL != handle->ptr){
            free(handle->ptr);
            free(handle);
            l_ret=0;
        }
        else {
            l_ret=0;
        }
    }
    return(l_ret);
}

int bkv_get_head(bkv_t h, 
                 uint8_t **p_str,
                 int      *p_strlen){
    if ((NULL == h) || (NULL == p_str)||(NULL == p_strlen)){
        return(BKV_INV_ARG);
    }
    /* @TODO forbid get_head when final map has not been closed. */
    *p_str=h->ptr;
    *p_strlen=h->write_offset;
    return(BKV_OK);
}

static int bkv_prepare(bkv_t h, int size_in_bits){
    int         l_new_size;
    int         l_append_size;
    uint8_t    *l_ptr;
    bkv_error_t l_ret=BKV_INV_ARG;

    PRINT_DBG(" mem_size %d write_offset %d size_in_bits %d",
              h->mem_size,
              h->write_offset,
              size_in_bits);

    if (h->mem_size < (h->write_offset + (size_in_bits>>3))){
        /* need to append memory. */
        l_append_size=(h->mem_chunk_size==-1)?BKV_DEFAULT_MEM_CHUNK_SIZE:h->mem_chunk_size;
        l_new_size=((h->mem_size/l_append_size)+1)*l_append_size;
        if ((-1 != h->max_size) && 
            (l_new_size > h->max_size)){
        }
#ifdef BKV_USE_POSIX_FILE_ACCESS
        else if (-1 != h->fd){
            if (BKV_OK != (l_ret = bkv_file_create_and_mmap(h->fd,l_new_size,&l_ptr))){
            }
        }
#endif
        else if (BKV_OK != (l_ret = bkv_mem_truncate(h->ptr,l_new_size,&l_ptr))){
        }
        if (BKV_OK == l_ret){
            h->ptr=l_ptr;
            h->mem_size=l_new_size;
        }
    }
    return(l_ret);
}

int bkv_write_offset_set(bkv_t handle, 
                         bkv_val_t *p_val){
    int          l_offset,l_ret=BKV_INV_ARG;
    bkv_t        l_end_bkv_hdl=BKV_INIT;
    bkv_create_t l_create_params=BKV_CREATE_INIT;

    BKV_FCT_INIT(bkv_write_offset_set);
    if ((NULL == handle)||(NULL==p_val)){
    }
    /* forbids write_offset set in case we already have a bkv end handle opened. */
    else if (BKV_INVALID != handle->end_hdl){
#ifdef DEBUG
            lg_print(LG_LEVEL_ERROR,BKV_FCT_NAME,__LINE__,
                     "End Handle already created");
#endif
    }
    else{
        p_val->priv+=p_val->bkv_key_length;
        l_offset=(int)((uint8_t*)p_val->priv - handle->ptr);
        if (l_offset < 0){
#ifdef DEBUG
            lg_print(LG_LEVEL_ERROR,BKV_FCT_NAME,__LINE__,
                     "Pointer of our current bkv");
#endif
        }
        else if (l_offset > handle->mem_size){
#ifdef DEBUG
            lg_print(LG_LEVEL_ERROR,BKV_FCT_NAME,__LINE__,
                     "Pointer of our current bkv offset %d memsize %d",l_offset,handle->mem_size);
#endif
        }
        else {
            l_create_params.create_type=BKV_CREATE_TYPE_UNSAFE_INPUT_BUFFER;
            l_create_params.input_ptr=(uint8_t*)p_val->priv;
            l_create_params.input_ptr_length=handle->mem_size-l_offset;
            if (BKV_OK != bkv_create(&l_create_params,
                                     &l_end_bkv_hdl)){
            }
            else {
                handle->end_hdl=l_end_bkv_hdl;
                handle->write_offset=l_offset;
                handle->deep_offset=p_val->deep_offset;
                /* TBD: ?? */
                handle->deep_value[handle->deep_offset]= '{';
                handle->state=STATE_IN_MAP;
                l_ret=BKV_OK;
            }
        }
    }
    return(l_ret);
}

bool bkv_is_inertion_mode_active(bkv_t h){
    return(NULL != h->end_hdl);
}


static void set_key(int       hdr_type,
                    uint16_t  key,
                    uint8_t  *p_ptr,
                    int      *p_offset){
    p_ptr[0] = (((hdr_type)<<4) & 0xFF) | ((key >> 8) & 0xFF);
    p_ptr[1] = (key & 0xFF);
    (*p_offset)+=2;
}

int bkv_kv_u16_add(bkv_t h,uint16_t key, uint16_t v){
    int l_ret=BKV_OK;

    BKV_FCT_INIT(bkv_kv_u16_add);
    BKV_HDL(h);

    if (key > 0xFFF){
        return(BKV_KEY_OUT_OF_RANGE);
    }
    if (-1 == bkv_check_state(h->state,h->deep_value[h->deep_offset],SET_KEY)){
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,16)){
        return(BKV_INV_STATE);
    }
    set_key(HDR_TYPE_INT16,key,&h->ptr[h->write_offset],&h->write_offset);
    cpu_to_le16(v,&h->ptr[h->write_offset],&h->write_offset);
    return(l_ret);
}

int bkv_kv_u32_add(bkv_t h,uint16_t key, uint32_t v){
    int l_ret=BKV_OK;

    BKV_FCT_INIT(bkv_kv_u32_add);
    BKV_HDL(h);

    if (key > 0xFFF){
        return(BKV_KEY_OUT_OF_RANGE);
    }
    if (-1 == bkv_check_state(h->state,h->deep_value[h->deep_offset],SET_KEY)){
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,32)){
        return(BKV_INV_STATE);
    }
    set_key(HDR_TYPE_INT32,key,&h->ptr[h->write_offset],&h->write_offset);
    cpu_to_le32(v,&h->ptr[h->write_offset],&h->write_offset);
    return(l_ret);
}

int bkv_kv_float_add(bkv_t h,uint16_t key, float v){
    int l_ret=BKV_OK;

    BKV_FCT_INIT(bkv_kv_float_add);
    BKV_HDL(h);

    if (key > 0xFFF){
        return(BKV_KEY_OUT_OF_RANGE);
    }
    if (-1 == bkv_check_state(h->state,h->deep_value[h->deep_offset],SET_KEY)){
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,48)){
        return(BKV_INV_STATE);
    }
    set_key(HDR_TYPE_FLOAT,key,&h->ptr[h->write_offset],&h->write_offset);
    cpu_to_lef(v,&h->ptr[h->write_offset],&h->write_offset);
    return(l_ret);
}

int bkv_kv_boolean_add(bkv_t h,uint16_t key, bool v){
    int l_ret=BKV_OK;

    BKV_FCT_INIT(bkv_kv_boolean_add);
    BKV_HDL(h);

    if (key > 0xFFF){
        return(BKV_KEY_OUT_OF_RANGE);
    }
    if (-1 == bkv_check_state(h->state,h->deep_value[h->deep_offset],SET_KEY)){
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,48)){
        return(BKV_INV_STATE);
    }
    set_key(HDR_TYPE_BOOLEAN,key,&h->ptr[h->write_offset],&h->write_offset);
    h->ptr[h->write_offset] = v?1:0;
    h->write_offset++;
    return(l_ret);
}

#define BKV_KV_TYPE_ADD(mtype,lower_type,UPPER_TYPE,size)     \
int bkv_kv_##lower_type##_add(bkv_t h,uint16_t key, mtype v){\
    int l_ret=BKV_OK;\
\
    BKV_FCT_INIT(bkv_kv_##lower_type##_add);\
    BKV_HDL(h);\
\
    if (key > 0xFFF){\
        return(BKV_KEY_OUT_OF_RANGE);\
    }\
    if (-1 == bkv_check_state(h->state,h->deep_value[h->deep_offset],SET_KEY)){\
        return(BKV_HDL_INV);\
    }\
    if (-1 == bkv_prepare(h,size)){\
        return(BKV_INV_STATE);\
    }\
    set_key(HDR_TYPE_##UPPER_TYPE,key,&h->ptr[h->write_offset],&h->write_offset);\
    cpu_to_le_##lower_type(v,&h->ptr[h->write_offset],&h->write_offset);\
    return(l_ret);\
}

BKV_KV_TYPE_ADD(double,double,DOUBLE,80)
BKV_KV_TYPE_ADD(uint64_t,u64,INT64,80)

int bkv_kv_str_add(bkv_t h,uint16_t key, const uint8_t *str, int len){
    int l_ret=BKV_OK;
    if (key > 0xFFF){
        return(BKV_KEY_OUT_OF_RANGE);
    }
    if (-1 == bkv_check_state(h->state,h->deep_value[h->deep_offset],SET_KEY)){
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,(HDR_SIZE + STRING_VALUE_SIZE +len)*8)){
        return(BKV_INV_STATE);
    }
#ifdef DEBUG
    lg_print(LG_LEVEL_INFO,BKV_FCT_NAME,__LINE__,
                          "write_offset=%d size=%d",h->write_offset,h->mem_size);
#endif
    set_key(HDR_TYPE_STRING,key,&h->ptr[h->write_offset],&h->write_offset);
    cpu_to_le16(len,&h->ptr[h->write_offset],&h->write_offset);
    memcpy(&h->ptr[h->write_offset],str,len);
    h->write_offset+=len;
    return(l_ret);
}

int bkv_kv_wchar_add(bkv_t h,uint16_t key, const wchar_t *str, int len){
    int l_ret=BKV_OK;
    if (key > 0xFFF){
        return(BKV_KEY_OUT_OF_RANGE);
    }
    if (-1 == bkv_check_state(h->state,h->deep_value[h->deep_offset],SET_KEY)){
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,(HDR_SIZE + STRING_VALUE_SIZE +len)*8)){
        return(BKV_INV_STATE);
    }
#ifdef DEBUG
    lg_print(LG_LEVEL_INFO,BKV_FCT_NAME,__LINE__,
                          "write_offset=%d size=%d",h->write_offset,h->mem_size);
#endif
    set_key(HDR_TYPE_STRING,key,&h->ptr[h->write_offset],&h->write_offset);
    cpu_to_le16(len*sizeof(wchar_t),&h->ptr[h->write_offset],&h->write_offset);
    memcpy(&h->ptr[h->write_offset],str,len);
    h->write_offset+=len*sizeof(wchar_t);
    return(l_ret);
}

int bkv_kv_map_open(bkv_t h, bkv_key_t key){
    int l_ret=BKV_OK;
    BKV_FCT_INIT(bkv_kv_map_open);
    BKV_HDL(h);
    if (-1 == bkv_check_state(h->state,(-1==h->deep_offset)?-1:h->deep_value[h->deep_offset],OPEN_MAP)){
#ifdef DEBUG
        lg_print(LG_LEVEL_ERROR,BKV_FCT_NAME,__LINE__,"invalid state");
#endif
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,32)){
#ifdef DEBUG
        lg_print(LG_LEVEL_ERROR,BKV_FCT_NAME,__LINE__,"Failed to prepare bhv");
#endif
        return(BKV_INV_STATE);
    }
#ifdef DEBUG
    lg_print(LG_LEVEL_INFO,BKV_FCT_NAME,__LINE__,
                          "write_offset=%d size=%d",h->write_offset,h->mem_size);
#endif

    h->map_offset[h->deep_offset+1]=h->write_offset;
    set_key(HDR_TYPE_MAP_OPEN,key,&h->ptr[h->write_offset],&h->write_offset);
    /* keep space to log map size.*/
    h->write_offset+=MAP_OPEN_VALUE_SIZE;
    h->deep_value[++h->deep_offset]=MAP_KEY;
    h->state=STATE_IN_MAP;
    return(l_ret);
}


int bkv_kv_map_close(bkv_t h){
    int l_ret=BKV_OK;
    uint16_t l_map_length;
    BKV_FCT_INIT(bkv_kv_map_close);
    BKV_HDL(h);
    if (-1 == bkv_check_state(h->state,h->deep_value[h->deep_offset],CLOSE_MAP)){
        return(BKV_HDL_INV);
    }

    if (-1 == bkv_prepare(h,64)){
        return(BKV_INV_STATE);
    }
#ifdef DEBUG
    lg_print(LG_LEVEL_INFO,BKV_FCT_NAME,__LINE__,
                          "write_offset=%d size=%d",h->write_offset,h->mem_size);
#endif

    h->ptr[h->write_offset] = (((HDR_TYPE_MAP_CLOSE)<<4) & 0xFF) ;
    h->write_offset+=HDR_SIZE;
    h->write_offset+=MAP_CLOSE_VALUE_SIZE;
    l_map_length=h->write_offset-h->map_offset[h->deep_offset];
    cpu_to_le16(l_map_length,
                &h->ptr[h->map_offset[h->deep_offset]+HDR_SIZE],
                NULL);
    h->deep_offset--;
    assert(h->deep_offset >= -1);
    if (-1 == h->deep_offset){
        h->state=STATE_TERM;
    }
    else {
        h->state=(h->deep_value[h->deep_offset]==MAP_KEY)?STATE_IN_MAP:STATE_IN_ARRAY;
    }
#ifdef DEBUG
    bkv_print_bkv(LG_LEVEL_DEBUG,BKV_FCT_NAME,line,h);
#endif
    return(l_ret);
}

int bkv_kv_array_open(bkv_t h, bkv_key_t key){
    int l_ret=BKV_OK;
    BKV_FCT_INIT(bkv_kv_array_open);
    BKV_HDL(h);
    if (-1 == bkv_check_state(h->state,(-1==h->deep_offset)?-1:h->deep_value[h->deep_offset],OPEN_ARRAY)){
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,16)){
        return(BKV_INV_STATE);
    }
#ifdef DEBUG
    lg_print(LG_LEVEL_INFO,BKV_FCT_NAME,__LINE__,
                          "write_offset=%d size=%d",h->write_offset,h->mem_size);
#endif

    h->map_offset[h->deep_offset+1]=h->write_offset;
    set_key(HDR_TYPE_ARRAY_OPEN,key,&h->ptr[h->write_offset],&h->write_offset);
    h->write_offset+=ARRAY_OPEN_VALUE_SIZE;
    h->deep_value[++h->deep_offset]=ARRAY_KEY;
    h->state=STATE_IN_ARRAY;
    return(l_ret);
}

#define BKV_KV_ARRAY_TYPE_ADD(mtype,lower_type,UPPER_TYPE,size)     \
int bkv_kv_array_##lower_type##_add(bkv_t h, mtype v){\
    int l_ret=BKV_OK;\
\
    BKV_FCT_INIT(bkv_kv_##lower_type##_add);\
    BKV_HDL(h);\
\
    if (-1 == bkv_check_state(h->state,h->deep_value[h->deep_offset],SET_ARRAY_KEY)){\
        return(BKV_HDL_INV);\
    }\
    if (-1 == bkv_prepare(h,size)){\
        return(BKV_INV_STATE);\
    }\
    set_key(HDR_TYPE_##UPPER_TYPE,BKV_ARRAY_KEY,&h->ptr[h->write_offset],&h->write_offset);\
    cpu_to_le_##lower_type(v,&h->ptr[h->write_offset],&h->write_offset);\
    return(l_ret);\
}


BKV_KV_ARRAY_TYPE_ADD(uint64_t,u64,INT64,80)


int bkv_kv_array_float_add(bkv_t h, float f){
    int l_ret=BKV_OK;
    BKV_FCT_INIT(bkv_kv_array_float_add);
    BKV_HDL(h);
    if (-1 == bkv_check_state(h->state,(-1==h->deep_offset)?-1:h->deep_value[h->deep_offset],SET_ARRAY_KEY)){
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,48)){
        return(BKV_INV_STATE);
    }
    set_key(HDR_TYPE_FLOAT,BKV_ARRAY_KEY,&h->ptr[h->write_offset],&h->write_offset);
    cpu_to_lef(f,&h->ptr[h->write_offset],&h->write_offset);
    h->array_nb_elems[h->deep_offset] ++;
    return(l_ret);
}

int bkv_kv_array_boolean_add(bkv_t h, bool f){
    int l_ret=BKV_OK;
    BKV_FCT_INIT(bkv_kv_array_boolean_add);
    BKV_HDL(h);
    if (-1 == bkv_check_state(h->state,(-1==h->deep_offset)?-1:h->deep_value[h->deep_offset],SET_ARRAY_KEY)){
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,48)){
        return(BKV_INV_STATE);
    }
    set_key(HDR_TYPE_BOOLEAN,BKV_ARRAY_KEY,&h->ptr[h->write_offset],&h->write_offset);
    h->ptr[h->write_offset]= f?1:0;
    h->write_offset++;
    h->array_nb_elems[h->deep_offset] ++;
    return(l_ret);
}

int bkv_kv_array_str_add(bkv_t h, const uint8_t *str, int len){
    int l_ret=BKV_OK;
    if (-1 == bkv_check_state(h->state,h->deep_value[h->deep_offset],SET_ARRAY_KEY)){
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,(HDR_SIZE + STRING_VALUE_SIZE +len)*8)){
        return(BKV_INV_STATE);
    }
#ifdef DEBUG
    lg_print(LG_LEVEL_INFO,BKV_FCT_NAME,__LINE__,
                          "write_offset=%d size=%d",h->write_offset,h->mem_size);
#endif
    set_key(HDR_TYPE_STRING,BKV_ARRAY_KEY,&h->ptr[h->write_offset],&h->write_offset);
    cpu_to_le16(len,&h->ptr[h->write_offset],&h->write_offset);
    memcpy(&h->ptr[h->write_offset],str,len);
    h->write_offset+=len;
    h->array_nb_elems[h->deep_offset] ++;
    return(l_ret);
}


int bkv_kv_array_u16_add(bkv_t h, uint16_t u){
    int l_ret=BKV_OK;
    BKV_FCT_INIT(bkv_kv_array_u16_add);
    BKV_HDL(h);
    if (-1 == bkv_check_state(h->state,(-1==h->deep_offset)?-1:h->deep_value[h->deep_offset],SET_ARRAY_KEY)){
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,32)){
        return(BKV_INV_STATE);
    }
    set_key(HDR_TYPE_INT16,BKV_ARRAY_KEY,&h->ptr[h->write_offset],&h->write_offset);
    cpu_to_le16(u,&h->ptr[h->write_offset],&h->write_offset);
    h->array_nb_elems[h->deep_offset] ++;
    return(l_ret);
}



int bkv_kv_array_close(bkv_t h){
    int l_ret=BKV_OK;
    uint16_t l_map_length;
    BKV_FCT_INIT(bkv_kv_map_close);
    BKV_HDL(h);
    if (-1 == bkv_check_state(h->state,h->deep_value[h->deep_offset],CLOSE_ARRAY)){
        return(BKV_HDL_INV);
    }
    if (-1 == bkv_prepare(h,16)){
        return(BKV_INV_STATE);
    }
#ifdef DEBUG
    lg_print(LG_LEVEL_INFO,BKV_FCT_NAME,__LINE__,
                          "write_offset=%d size=%d",h->write_offset,h->mem_size);
#endif

    h->ptr[h->write_offset] = (((HDR_TYPE_ARRAY_CLOSE)<<4) & 0xFF) ;
    h->write_offset+=HDR_SIZE;
    h->ptr[h->write_offset]=0xee;
    h->ptr[h->write_offset+1]=0xee;
    h->ptr[h->write_offset+2]=0xee;
    h->write_offset+=ARRAY_CLOSE_VALUE_SIZE;
    l_map_length=h->write_offset-h->map_offset[h->deep_offset];
    /* set the number of elements of the array in its header. */
    cpu_to_le16(h->array_nb_elems[h->deep_offset],
                &h->ptr[h->map_offset[h->deep_offset]+HDR_SIZE+2],
                NULL);
    /* set the length in bytes of the array. */
    cpu_to_le16(l_map_length,
                &h->ptr[h->map_offset[h->deep_offset]+HDR_SIZE],
                NULL);
    h->deep_value[h->deep_offset]=0;
    h->deep_offset--;
    assert(h->deep_offset >= -1);
    if (-1 == h->deep_offset){
        assert(-1);
    }
    else {
        h->state=(h->deep_value[h->deep_offset]==MAP_KEY)?STATE_IN_MAP:STATE_IN_ARRAY;
    }
#ifdef DEBUG
    bkv_print_bkv(LG_LEVEL_DEBUG,BKV_FCT_NAME,line,h);
#endif
    return(l_ret);
}



int bkv_append(bkv_t h, bkv_t a){
    uint8_t *l_ptr;
    int      l_size;

    BKV_FCT_INIT(bkv_append);
    BKV_HDL(h);

    l_size = h->write_offset+a->write_offset;
    if (l_size > h->mem_size){
        l_size = ((l_size * BKV_DEFAULT_MEM_CHUNK_SIZE)/BKV_DEFAULT_MEM_CHUNK_SIZE);
        //l_size = (((l_size / BKV_DEFAULT_MEM_CHUNK_SIZE) + 1) *BKV_DEFAULT_MEM_CHUNK_SIZE);
#ifdef BKV_USE_POSIX_FILE_ACCESS
        if (-1 != h->fd){
            if (BKV_OK != bkv_file_truncate(h->fd,l_size)){
                return(BKV_HDL_INV);
            }
            l_ptr=h->ptr;
            /* mmap.*/
        } else
#endif
            if (NULL == (l_ptr = realloc(h->ptr,l_size))){
            return(BKV_HDL_MEM);
        }
        h->ptr=l_ptr;
        h->mem_size=l_size;
    }
    memcpy(&h->ptr[h->write_offset],&a->ptr[0],a->write_offset);
    h->write_offset+=a->write_offset;
    return(BKV_OK);
}

int bkv_sync(bkv_t handle){
    assert(handle);
#ifdef BKV_USE_POSIX_FILE_ACCESS
    if (handle->fd != -1){
        bkv_file_sync(handle->fd,handle->ptr);
    }
#endif
    return(0);
}

#ifdef DEBUG
int bkv_print_state(const char *p_function, int line, bkv_t h){
    lg_print(p_function,line," HDL:%p  state %d deep_offset %d\n",(void*)h,h->state,h->deep_offset);
    return(0);
}

int bkv_print_bkv(const char *p_function, int line, bkv_t h){
    int     i;
    int     l_ret=0;
#if 0
    int     j;
    char    l_buffer[64];
    int     l_offset=0;
#endif
    for(i=0;i<h->mem_size;i+=8){
        lg_print(p_function,line," i:  %3d  : %2x %2x %2x %2x  %2x %2x %2x %2x\n",
                  i,
                  h->ptr[i],
                  h->ptr[i+1],
                  h->ptr[i+2],
                  h->ptr[i+3],
                  h->ptr[i+4],
                  h->ptr[i+5],
                  h->ptr[i+6],
                  h->ptr[i+7]);

#if 0
        l_offset=0;
        l_offset+=snprintf((char*)&l_buffer[l_offset],sizeof(l_buffer)-l_offset," i:  %3d  : ",i);
        for(j=0;j<8;j++){
            if ((i+j)<h->mem_size){
                if (isalpha(h->ptr[i+j])){
                    l_ret=snprintf(&l_buffer[l_offset],sizeof(l_buffer)-l_offset,"%2c ",h->ptr[i+j]);
                }
                else {
                    l_ret=snprintf(&l_buffer[l_offset],sizeof(l_buffer)-l_offset,"%s",".. ");
                }

                if (-1 == l_ret){
                    break;
                }
                else {
                    l_offset+=l_ret;
                }
                if (l_offset > sizeof(l_buffer)){
                    lg_print(p_function,line,"%s",&l_buffer[0]);
                    break;
                }
            }
            else {
                goto end;
            }
        }
        snprintf(&l_buffer[l_offset],sizeof(l_buffer)-l_offset,"%s","\n");
        lg_print(p_function,line,"%s",&l_buffer[0]);
#endif
    }
#if 0
end:
#endif
    return(l_ret);
}
#endif
