#include <stdlib.h>
#include <stdio.h>
#include "dico_plugs.h"

static bkv_error_t dico_direct_create(bkv_dico_create_t *p_create, void **l_priv);
static bkv_error_t dico_direct_key_add(void *l_priv, bkv_key_t key, bkv_key_t *p_final_key, const uint8_t *keystr, int keystrlen);
static bkv_error_t dico_direct_destroy(void *l_priv, bkv_t *p_dico_handle);

typedef struct {
    bkv_t handle;
}dico_direct_ctx_t;

dico_plug_t s_dico_direct_plug={
    dico_direct_create,
    dico_direct_key_add,
    dico_direct_destroy
} ;


static bkv_error_t dico_direct_create(bkv_dico_create_t *p_create, void **l_priv){
    bkv_error_t         l_ret=BKV_INV_ARG;
    dico_direct_ctx_t  *l_ctx=NULL;
    bkv_create_t        l_bkv_create=BKV_CREATE_INIT;
    bkv_t               l_bkv;

    (void)p_create;
    (void)l_priv;
    l_bkv_create.create_type=BKV_CREATE_TYPE_WORK_IN_RAM;
    if (BKV_OK != bkv_create(&l_bkv_create,&l_bkv)){
    }
    else if (BKV_OK != bkv_kv_map_open(l_bkv,BKV_DICO_KEY)){
    }
    else if (NULL == (l_ctx = malloc(sizeof(*l_ctx)))){
    } else {
        l_ctx->handle=l_bkv;
        *l_priv=l_ctx;
        l_ret=BKV_OK;
    }

    return(l_ret);
}

static bkv_error_t dico_direct_key_add(void *l_priv, 
                                       bkv_key_t key, 
                                       bkv_key_t *p_final_key,
                                       const uint8_t *keystr, 
                                       int keystrlen){
    dico_direct_ctx_t *l_ctx=(dico_direct_ctx_t*)l_priv;
    bkv_error_t        l_error=BKV_INV_ARG;
    if (NULL != p_final_key){
        *p_final_key=BKV_NO_KEY;
    }
    if (BKV_OK != (l_error = bkv_kv_str_add(l_ctx->handle,key,keystr,keystrlen))){
        printf("Failed to add key to direct dico");
    }
    return(l_error);
}
static bkv_error_t dico_direct_destroy(void *l_priv, bkv_t *p_dico_handle){
    bkv_error_t        l_ret=BKV_INV_ARG;
    dico_direct_ctx_t *l_ctx=(dico_direct_ctx_t*)l_priv;

    if ((NULL == l_ctx) || (NULL == p_dico_handle)){
    }
    else if (BKV_OK != (l_ret = bkv_kv_map_close(l_ctx->handle))){
    }
    else {
        *p_dico_handle=l_ctx->handle;
        free(l_priv);
        l_ret=BKV_OK;
    }
    return(l_ret);
}

