#include <stdlib.h>
#include <stdio.h>
#include "dico_plugs.h"
#include "bkv_dico.h"

typedef struct {
    bkv_dico_type_t  type;
    dico_plug_t     *plug;
}dico_plugs_t;

static dico_plugs_t s_dico_plugs[]={
    { BKV_DICO_TYPE_DIRECT,&s_dico_direct_plug},
    { BKV_DICO_TYPE_CACHED,&s_dico_cached_plug},
    { BKV_DICO_TYPE_END_VALUE,NULL}
};

static dico_plug_t *dico_get_plug(bkv_dico_type_t type){
    int i=0;
    while((s_dico_plugs[i].type != BKV_DICO_TYPE_END_VALUE)&&
          (s_dico_plugs[i].type != type)){
        i++;
    }
    if (s_dico_plugs[i].type == BKV_DICO_TYPE_END_VALUE){
        return(NULL);
    }
    return(s_dico_plugs[i].plug);
}

int bkv_dico_create(bkv_dico_create_t *p_dico_create,
                    bkv_dico_t        *p_dico_handle){
    bkv_error_t      l_ret=BKV_INV_ARG;
    dico_ctx_t      *l_dico_ctx;
    dico_plug_t     *l_dico_plug;
    void            *l_priv_handle;
    if ((NULL == p_dico_create)||(NULL == p_dico_handle)){
    }
    else if (NULL == (l_dico_plug = dico_get_plug(p_dico_create->type))){
    }
    else if (BKV_OK != (l_ret = l_dico_plug->create(p_dico_create,
                                                    &l_priv_handle))){
    }
    else if (NULL == (l_dico_ctx = malloc(sizeof(*l_dico_ctx)))){
        printf(" Failed to allocate %d bytes",sizeof(*l_dico_ctx));
    }
    else{
        l_dico_ctx->plug=l_dico_plug;
        l_dico_ctx->priv_handle=l_priv_handle;
        *p_dico_handle=l_dico_ctx;
        l_ret=BKV_OK;
    }
    return(l_ret);
}

int bkv_dico_key_add(bkv_dico_t     dico_handle,
                     bkv_key_t      key_index,
                     const uint8_t *keystr,
                     int            keystrlen){
    dico_ctx_t      *l_dico_ctx=dico_handle;
    return(l_dico_ctx->plug->key_add(l_dico_ctx->priv_handle,key_index, keystr,keystrlen));
}
int bkv_dico_destroy(bkv_dico_t   dico_handle,
                     bkv_t       *p_bkv_dico_handle){
    dico_ctx_t      *l_dico_ctx=dico_handle;
    return(l_dico_ctx->plug->destroy(l_dico_ctx->priv_handle,p_bkv_dico_handle));
}
