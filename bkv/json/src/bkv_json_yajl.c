#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <yajl/yajl_parse.h>
#include "bkv_val.h"
#include "bkv_json_yajl.h"
#ifdef HAS_DICO
#include "bkv_dico.h"
#endif
#include "json_plugs.h"

typedef struct {
    bkv_key_t key_index;
    bkv_key_t existing_key_index;

} bkv_from_json_yajl_ctx_t;

static int bkv_from_json_yajl_parse(bkv_from_json_ctx_t *p_ctx, const uint8_t *ptr, int len);
static void *bkv_json_malloc(void *, size_t);
static void bkv_json_free(void *, void *);
static void *bkv_json_realloc(void *, void *, size_t);

static int bkv_to_json_yajl_init(bkv_to_json_ctx_t *p_ctx);
static int bkv_to_json_yajl_finalize(bkv_to_json_ctx_t *p_ctx, uint8_t **p_ptr, int *p_len);
static bkv_error_t bkv_to_json_yajl_map_open(bkv_to_json_ctx_t *p_ctx, const uint8_t *key, int keylen);
static int bkv_to_json_yajl_map_close(bkv_to_json_ctx_t *p_ctx);
static int bkv_to_json_yajl_array_open(bkv_to_json_ctx_t *p_ctx, int array_len, const uint8_t *keyname, int keylen);
static int bkv_to_json_yajl_array_close(bkv_to_json_ctx_t * ctx);
static int bkv_to_json_yajl_uint16(bkv_to_json_ctx_t *p_ctx, uint8_t *p_str, int strlen, uint16_t value);
static bkv_error_t bkv_to_json_yajl_float(bkv_to_json_ctx_t *p_ctx, uint8_t *str, int strlen, float f);
static bkv_error_t bkv_to_json_yajl_array_float(bkv_to_json_ctx_t *p_ctx, float f);
static int bkv_to_json_yajl_str(bkv_to_json_ctx_t *p_ctx, uint8_t *str, int strlen, uint8_t *p_str_value, int str_valuelen);

static struct bkv_to_json_parser_s s_bkv_to_json_yajl_plug = {
    bkv_to_json_yajl_init,
    bkv_to_json_yajl_map_open,
    bkv_to_json_yajl_map_close,
    bkv_to_json_yajl_array_open,
    bkv_to_json_yajl_array_close,
    bkv_to_json_yajl_uint16,
    bkv_to_json_yajl_float,
    bkv_to_json_yajl_array_float,
    bkv_to_json_yajl_str,
    bkv_to_json_yajl_finalize
};

static struct bkv_from_json_parser_s s_bkv_from_json_yajl_plug = {
    bkv_from_json_yajl_parse
};

static yajl_alloc_funcs s_alloc_funcs = {
    bkv_json_malloc,
    bkv_json_realloc,
    bkv_json_free,
    NULL
};


bkv_from_json_parser_t bkv_from_json_yajl_parser_get(void){
    return(&s_bkv_from_json_yajl_plug);
}

bkv_to_json_parser_t bkv_to_json_yajl_parser_get(void){
    return(&s_bkv_to_json_yajl_plug);
}

void bkv_from_json_yajl_parser_rel(bkv_from_json_parser_t p){
    (void)p;
}

void bkv_to_json_yajl_parser_rel(bkv_to_json_parser_t p){
    (void)p;
}

static int
bkv_json_get_boolean(void *ctx, int boolVal){
    bkv_from_json_ctx_t *l_ctx = (bkv_from_json_ctx_t*)ctx;
    (void)boolVal;
    if (NULL != l_ctx){
        printf(" Find Boolean\n");
    }
    return(1);
}

static int
bkv_json_get_integer(void *ctx, long long integerVal){
    bkv_from_json_ctx_t      *l_ctx = (bkv_from_json_ctx_t *) ctx;
    int                       l_ret=-1;
    (void)integerVal;
    if (NULL != l_ctx){
        printf(" Find Integer\n");
        l_ret=0;
    }

    return (l_ret);
}

static int
bkv_json_get_double(void *ctx, double doubleVal){
    bkv_from_json_ctx_t      *l_ctx = (bkv_from_json_ctx_t *) ctx;
    bkv_from_json_yajl_ctx_t *l_yajl_ctx=l_ctx->priv_data;
    bkv_key_t                 l_index;
    int                       l_ret;
    if (NULL != l_ctx){
        if (l_ctx->in_array){
            if (BKV_OK != (l_ret = bkv_kv_array_float_add(l_ctx->data_handle,doubleVal))){
                printf(" Failed to add array float\n");
            }
        }
        else {
            l_index=(l_yajl_ctx->existing_key_index!=BKV_NO_KEY)?l_yajl_ctx->existing_key_index:l_yajl_ctx->key_index;
            if (BKV_OK != (l_ret = bkv_kv_float_add(l_ctx->data_handle,l_index,doubleVal))){
                printf(" Failed to add array\n");
            }
        }
    }

    return (1);
}

static int
bkv_json_get_number(void *ctx, const char *numberVal, size_t numberLen){
    bkv_from_json_ctx_t      *l_ctx = (bkv_from_json_ctx_t *) ctx;
    bkv_from_json_yajl_ctx_t *l_yajl_ctx=l_ctx->priv_data;
    bkv_key_t                 l_index;
    float                     l_value;
    int                       l_ret;
    (void) numberLen;
    (void)l_ctx;
    (void)ctx;
    if ((NULL != l_ctx) && (NULL != numberVal)){
        l_value=strtof(numberVal,NULL);
        if (l_ctx->in_array){
            if (BKV_OK != (l_ret = bkv_kv_array_float_add(l_ctx->data_handle,l_value))){
                printf(" Failed to add array float\n");
            }
        }
        else {
            l_index=(l_yajl_ctx->existing_key_index!=BKV_NO_KEY)?l_yajl_ctx->existing_key_index:l_yajl_ctx->key_index;
            if (BKV_OK != (l_ret = bkv_kv_float_add(l_ctx->data_handle,l_index,l_value))){
                printf(" Failed to add array\n");
            }
        }
 
    }

    return (1);
}

static int
bkv_json_get_string(void *ctx,
                    const unsigned char *stringVal,
                    size_t stringLen){
    bkv_from_json_ctx_t      *l_ctx = (bkv_from_json_ctx_t *) ctx;
    bkv_from_json_yajl_ctx_t *l_yajl_ctx=l_ctx->priv_data;
    bkv_key_t                 l_index;
    int                       l_ret;
    (void)ctx;
    if ((NULL != l_ctx) && (NULL != stringVal)){
        l_index=(l_yajl_ctx->existing_key_index!=BKV_NO_KEY)?l_yajl_ctx->existing_key_index:l_yajl_ctx->key_index;
        if (-1 == (l_ret = bkv_kv_str_add(l_ctx->data_handle,l_index,stringVal,stringLen))){
        }
    }
    return (1);
}

static int
bkv_json_start_map(void * ctx){
    int                  l_ret=-1;
    bkv_key_t            l_index;
    bkv_from_json_ctx_t *l_ctx = (bkv_from_json_ctx_t *) ctx;
    bkv_from_json_yajl_ctx_t *l_yajl_ctx=l_ctx->priv_data;

    if (NULL != l_ctx){
        if (0 < l_ctx->deep){
            l_index=(l_yajl_ctx->existing_key_index!=BKV_NO_KEY)?l_yajl_ctx->existing_key_index:l_yajl_ctx->key_index;
            if (BKV_OK != (l_ret = bkv_kv_map_open(l_ctx->data_handle,l_index))){
                printf("Failed to add map with key %d\n",l_yajl_ctx->key_index);
            }
        }
        else {
            if (BKV_OK != (l_ret = bkv_kv_map_open(l_ctx->data_handle,BKV_NO_KEY))){
                printf("Failed to add nokey map\n");
            }
        }
        if (-1 != l_ret){
            l_ctx->deep++;
            l_ret=1;
        }
    }
    return(l_ret);
}

static int
bkv_json_end_map(void * ctx){
    int                       l_ret=-1;
    bkv_from_json_ctx_t      *l_ctx = (bkv_from_json_ctx_t *) ctx;
    if (NULL != l_ctx){
        l_ctx->deep--;
        if ((l_ctx->deep > 0) || (NULL == l_ctx->dico_handle)){
            if (-1 == (l_ret = bkv_kv_map_close(l_ctx->data_handle))){
            }
            else {
                l_ret=1;
            }
        }
    }
    return(l_ret);
}

static int
bkv_json_start_array(void * ctx){
    int                       l_ret=-1;
    bkv_key_t                 l_index;
    bkv_from_json_ctx_t      *l_ctx = (bkv_from_json_ctx_t *) ctx;
    bkv_from_json_yajl_ctx_t *l_yajl_ctx=l_ctx->priv_data;

    if (NULL != l_ctx){
        if (0 < l_ctx->deep){
            l_index=(l_yajl_ctx->existing_key_index!=BKV_NO_KEY)?l_yajl_ctx->existing_key_index:l_yajl_ctx->key_index;
            if (BKV_OK != (l_ret = bkv_kv_array_open(l_ctx->data_handle,l_index))){
                printf("Failed to add array with key %d\n",l_yajl_ctx->key_index);
            }
        }
        else {
            if (BKV_OK != (l_ret = bkv_kv_array_open(l_ctx->data_handle,BKV_NO_KEY))){
                printf("Failed to add nokey array\n");
            }
        }
        if (-1 != l_ret){
            l_ctx->in_array = true;
            l_ctx->deep++;
            l_ret=1;
        }
    }
    return(l_ret);
}

static int
bkv_json_end_array(void * ctx){
    int                       l_ret=-1;
    bkv_from_json_ctx_t      *l_ctx = (bkv_from_json_ctx_t *) ctx;
    if (NULL != l_ctx){
        l_ctx->deep--;
        if (l_ctx->deep > 0){
            if (-1 == (l_ret = bkv_kv_array_close(l_ctx->data_handle))){
            }
            else {
                l_ret=1;
                l_ctx->in_array = false;
            }
        }
    }
    return(l_ret);
}



static int
bkv_json_map_keys(void *ctx, const unsigned char *key, size_t keylen){
    bkv_error_t l_ret=BKV_INV_ARG;
    bkv_key_t   l_returned_index=BKV_NO_KEY;
    bkv_from_json_ctx_t      *l_ctx = (bkv_from_json_ctx_t *) ctx;
    bkv_from_json_yajl_ctx_t *l_yajl_ctx=l_ctx->priv_data;
    if (NULL != l_ctx){
#ifndef HAS_DICO
        if (BKV_OK != bkv_kv_str_add(l_ctx->dico_handle,++l_yajl_ctx->key_index,(const char *)key,keylen)){
            return(0);
        }
#else
        if (BKV_OK != (l_ret=bkv_dico_key_add(l_ctx->dico_handle,
                                              ++l_yajl_ctx->key_index,
                                              &l_returned_index,
                                              key,
                                              keylen))){
            return(l_ret);
        }
        else {
            if (l_returned_index != BKV_NO_KEY){
                l_yajl_ctx->existing_key_index=l_returned_index;
            }
            else {
                l_yajl_ctx->existing_key_index=BKV_NO_KEY;
            }
        }
#endif
    }
    return (l_ret);
}

static int
bkv_from_json_yajl_parse(bkv_from_json_ctx_t *p_ctx,
                         const uint8_t       *ptr,
                         int                  len){
    bkv_error_t              l_ret=BKV_INV_ARG;
    bkv_from_json_yajl_ctx_t l_ctx;
    yajl_handle              l_yajl_handle;
    yajl_status              l_status;
    yajl_callbacks           callbacks = {
        NULL,
        bkv_json_get_boolean,
        bkv_json_get_integer,
        bkv_json_get_double,
        bkv_json_get_number,
        bkv_json_get_string,
        bkv_json_start_map,
        bkv_json_map_keys,
        bkv_json_end_map,
        bkv_json_start_array,
        bkv_json_end_array
    };

    p_ctx->priv_data=&l_ctx;
    l_ctx.key_index=1;
    if ((l_yajl_handle = yajl_alloc(&callbacks, &s_alloc_funcs, p_ctx)) == NULL) {
        printf("Failed to allocate yajl handle");
        goto error;
    }
    if ((l_status = yajl_parse(l_yajl_handle, ptr, len)) != yajl_status_ok) {
        yajl_free(l_yajl_handle);
        printf("Failed to parse json. status = %d", l_status);
        goto error;
    }
    l_ret=BKV_OK;
error:
    yajl_free(l_yajl_handle);
    return (l_ret);
}

#include <yajl/yajl_gen.h>
typedef struct {
    yajl_handle          h;
    yajl_gen             g;
    bkv_val_t            dico_val;

} bkv_to_json_yajl_ctx_t;


static void *bkv_json_malloc(void *ctx, size_t sz)
{
    (void) ctx;
    return (malloc(sz));
}

static void bkv_json_free(void *ctx, void *ptr)
{
    (void) ctx;
    free(ptr);
}

static void *bkv_json_realloc(void *ctx, void *ptr, size_t sz)
{
    (void) ctx;
    return (realloc(ptr, sz));
}

static bkv_error_t
bkv_to_json_yajl_map_open(bkv_to_json_ctx_t *p_ctx,
                          const uint8_t     *key,
                          int                keylen){
    int                       l_ret=BKV_INV_ARG;
    bkv_to_json_yajl_ctx_t *l_ctx=(bkv_to_json_yajl_ctx_t*)p_ctx->priv_data;
    (void)key;
    (void)keylen;
    if (NULL != l_ctx){
        if ((NULL  != key) && (0 < keylen) &&
            (yajl_gen_status_ok != yajl_gen_string(l_ctx->g,(const unsigned char*)key,keylen))){
        }
        if (yajl_gen_status_ok == yajl_gen_map_open(l_ctx->g)){
            l_ret=BKV_OK;
        }
        else {
            printf("bkv_to_json_yajl_map_open :Failed to add map\n");
        }

    }
    return(l_ret);
}

static bkv_error_t
bkv_to_json_yajl_map_close(bkv_to_json_ctx_t *p_ctx){
    bkv_error_t             l_ret=BKV_OK;
    bkv_to_json_yajl_ctx_t *l_ctx=(bkv_to_json_yajl_ctx_t*)p_ctx->priv_data;
    if (NULL != l_ctx){
        if (yajl_gen_status_ok != yajl_gen_map_close(l_ctx->g)){
            printf("Failed to add map close\n");
            l_ret=BKV_INV_STATE;
        }

    }
    return(l_ret);
}

static bkv_error_t
bkv_to_json_yajl_array_open(bkv_to_json_ctx_t *p_ctx,
                            int                array_len,
                            const uint8_t     *keyname,
                            int                keylen) {
    bkv_error_t   l_ret=BKV_OK;
    bkv_to_json_yajl_ctx_t *l_ctx=(bkv_to_json_yajl_ctx_t*)p_ctx->priv_data;
    (void)array_len;
    if (NULL != l_ctx){
        if (yajl_gen_status_ok != yajl_gen_string(l_ctx->g,keyname,keylen)){
            l_ret=BKV_INV_STATE;
        } 
        else if (yajl_gen_status_ok != yajl_gen_array_open(l_ctx->g)){
            l_ret=BKV_INV_STATE;
            printf("Failed to add array open\n");
        }
    }
    return(l_ret);
}

static bkv_error_t
bkv_to_json_yajl_array_close(bkv_to_json_ctx_t *p_ctx) {
    bkv_error_t   l_ret=BKV_INV_ARG;
    bkv_to_json_yajl_ctx_t *l_ctx=(bkv_to_json_yajl_ctx_t*)p_ctx->priv_data;
    if (NULL != l_ctx){
        if (yajl_gen_status_ok != yajl_gen_array_close(l_ctx->g)){
            printf("Failed to add close array\n");
        }
        else {
            l_ret=BKV_OK;
        }
    }
    return(l_ret);
}



#if 0
static int
bkv_json_map_key(void *p_data, bkv_key_t key){
    int                       l_ret=0;
    (void)p_data;
    (void)key;
#if 0
    bkv_to_json_yajl_ctx_t *l_ctx=(bkv_to_json_yajl_ctx_t*)p_data;
    bkv_key_t                 l_keys[2]= { key, BKV_KEY_INVALID};
    bkv_val_t                 l_val;
    (void)key;
    if (NULL != l_ctx){
        if (BKV_OK != bkv_val_get(&l_ctx->dico_val,&l_keys[0],&l_val)){
        }
        else if (yajl_gen_status_ok == yajl_gen_string(l_ctx->g,
                                                       l_val.u.string.str,
                                                       l_val.u.string.len)){
            l_ret=1;
        }
    }
#endif
    return(l_ret);
}
#endif

static int
bkv_to_json_yajl_uint16(bkv_to_json_ctx_t *p_ctx, uint8_t *str, int strlen, uint16_t value){
    bkv_to_json_yajl_ctx_t *l_ctx=(bkv_to_json_yajl_ctx_t*)p_ctx->priv_data;
    int                       l_ret=BKV_OK;

    if (NULL != l_ctx){
        if (yajl_gen_status_ok != yajl_gen_string(l_ctx->g,str,strlen)){
            l_ret=BKV_INV_ARG;
        }
        else if (yajl_gen_status_ok != yajl_gen_integer(l_ctx->g, (long long int)value)){
            l_ret=BKV_INV_ARG;
        }
    }
    return(l_ret);
}


static int
bkv_to_json_yajl_str(bkv_to_json_ctx_t *p_ctx, uint8_t *str, int strlen, uint8_t *p_str_value, int str_valuelen){
    bkv_to_json_yajl_ctx_t *l_ctx=(bkv_to_json_yajl_ctx_t*)p_ctx->priv_data;
    int                       l_ret=BKV_OK;

    if (NULL != l_ctx){
        if (yajl_gen_status_ok != yajl_gen_string(l_ctx->g,str,strlen)){
            l_ret=BKV_INV_ARG;
        }
        else if (yajl_gen_status_ok != yajl_gen_string(l_ctx->g, p_str_value, str_valuelen)){
            l_ret=BKV_INV_ARG;
        }
    }
    return(l_ret);
}


static bkv_error_t
bkv_to_json_yajl_float(bkv_to_json_ctx_t *p_ctx, uint8_t *str, int strlen, float f){
    bkv_to_json_yajl_ctx_t *l_ctx=(bkv_to_json_yajl_ctx_t*)p_ctx->priv_data;
    int                       l_ret=BKV_OK;

    if (NULL != l_ctx){
        if (yajl_gen_status_ok != yajl_gen_string(l_ctx->g,str,strlen)){
            l_ret=BKV_INV_ARG;
        }
        else if (yajl_gen_status_ok != yajl_gen_double(l_ctx->g, (double)f)){
            l_ret=BKV_INV_ARG;
        }
    }
    return(l_ret);
}

static bkv_error_t
bkv_to_json_yajl_array_float(bkv_to_json_ctx_t *p_ctx, float f){
    bkv_to_json_yajl_ctx_t *l_ctx=(bkv_to_json_yajl_ctx_t*)p_ctx->priv_data;
    int                       l_ret=BKV_OK;

    if (NULL != l_ctx){
        if (yajl_gen_status_ok != yajl_gen_double(l_ctx->g, (double)f)){
            l_ret=BKV_INV_ARG;
        }
    }
    return(l_ret);
}

static int
bkv_to_json_yajl_init(bkv_to_json_ctx_t *p_ctx){
    int                       l_ret=-1;
    bkv_to_json_yajl_ctx_t *l_ctx=p_ctx->priv_data;
    if (NULL == (l_ctx = malloc(sizeof(*l_ctx)))){
        printf("Failed to allocate %zu bytes\n",sizeof(*l_ctx));
    }
    else  if (NULL == (l_ctx->g = yajl_gen_alloc(NULL))) {
        printf("Failed to allocate gen handle\n");
    }
    else if(0 == (l_ret = yajl_gen_config(l_ctx->g, yajl_gen_validate_utf8, 1))) {
        printf("Failed to configure yajl generator\n");
    }
    else if (NULL == ( l_ctx->h = yajl_alloc(NULL, NULL, (void *) l_ctx->g))) {
        printf("Failed to allocate yajl handle\n");
    }
    else if(!(l_ret = yajl_config(l_ctx->h, yajl_allow_comments, 1))) {
        printf("Failed to configure handle\n");
    }
    else {
        p_ctx->priv_data = l_ctx;
        l_ret=0;
    }
    return(l_ret);
}

static int
bkv_to_json_yajl_finalize(bkv_to_json_ctx_t *p_ctx,
                            uint8_t **p_str,
                            int      *p_str_len){
    uint8_t                  *l_ptr;
    int                       l_ret=BKV_INV_ARG;
    size_t                    l_len;
    bkv_to_json_yajl_ctx_t *l_ctx=p_ctx->priv_data;

    if((l_ret = yajl_gen_get_buf(l_ctx->g, (const unsigned char **)&l_ptr, &l_len))) {
        printf("Failed to generate buffer \n");
    }
    else{
        *p_str=malloc(l_len);
        memcpy(*p_str,l_ptr,l_len);
        *p_str_len=l_len;
        yajl_free(l_ctx->h);
        yajl_gen_free(l_ctx->g);
        free(l_ctx);

        l_ret=BKV_OK;
    }
    return(l_ret);
}


