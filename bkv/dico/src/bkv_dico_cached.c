#include <stdlib.h>
#include <stdio.h>
#include "keytree.h"
#include "dico_plugs.h"


static bkv_error_t dico_cached_create(bkv_dico_create_t *p_create, void **l_priv);
static bkv_error_t dico_cached_key_add(void *l_priv, bkv_key_t key, bkv_key_t *final_key, const uint8_t *keystr, int keystrlen);
static bkv_error_t dico_cached_destroy(void *l_priv, bkv_t *p_bkv_dico_handle);

typedef struct {
    keytree_head_t *head;
} dico_cached_ctx_t;

typedef struct {
    bkv_t dico;
    bkv_key_t key_index;
} dico_cached_lookup_ctx_t;

dico_plug_t s_dico_cached_plug={
    dico_cached_create,
    dico_cached_key_add,
    dico_cached_destroy
} ;


static bkv_error_t dico_cached_create(bkv_dico_create_t *p_create, void **l_priv){
    keytree_head_t *l_head;
    keytree_list_t  l_list=WG_KEY_INIT;
    dico_cached_ctx_t *l_dico;
    bkv_error_t        l_ret=BKV_INV_ARG;

    (void)p_create;
    (void)l_priv;
    if (W_NO_ERROR != wg_keytree_create(&l_list,&l_head)){
    }
    else if (NULL == (l_dico = malloc(sizeof(*l_dico)))){
    }
    else {
        l_dico->head=l_head;
        *l_priv=l_dico;
        l_ret=BKV_OK;
    }
    return(l_ret);
}

static bkv_error_t dico_cached_key_add(void *l_priv, bkv_key_t key, bkv_key_t *final_key, const uint8_t *keystr, int keystrlen){
    keytree_elem_t            l_elem;
    dico_cached_ctx_t        *l_ctx=(dico_cached_ctx_t*)l_priv;
    void                     *l_priv_data=NULL;
    w_error_t                 l_error;
    (void)l_priv;
    (void)key;
    (void)keystr;
    (void)keystrlen;
    l_elem.w.len=keystrlen;
    l_elem.w.str=(uint8_t*)strdup(keystr);
    l_elem.udata=(void*)(int)key;
    l_error = wg_keytree_add(l_ctx->head,&l_elem,&l_priv_data);
    switch(l_error){
    case W_E_EXISTS:
        if (NULL != final_key){
            *final_key=(bkv_key_t)(int)l_priv_data;
        }
        break;
    default:
        break;
    }
    return(BKV_OK);
}

static w_error_t dico_cached_elem(wg_key_t      *p_key, 
                                  const uint8_t *p_str,
                                  int            strlen,
                                  void          *p_data){
    w_error_t l_ret=W_E_BAD_PARAMETER;
    dico_cached_lookup_ctx_t *l_lookup_ctx=(dico_cached_lookup_ctx_t*)p_data;

    if (true == p_key->end_of_word){
        if (BKV_OK != bkv_kv_str_add(l_lookup_ctx->dico,(bkv_key_t)(int)p_key->udata,p_str,strlen)){
            printf("Failed to add a string to cached dico\n");
        }
        else {
            l_ret=W_NO_ERROR;
        }
    }

    return(l_ret);
}

static bkv_error_t dico_cached_destroy(void *l_priv, bkv_t *p_bkv_dico_handle){
    bkv_error_t               l_ret=BKV_INV_ARG;
    dico_cached_ctx_t        *l_ctx=(dico_cached_ctx_t*)l_priv;
    bkv_create_t              l_bkv_create=BKV_CREATE_INIT;
    dico_cached_lookup_ctx_t  l_lookup_ctx;
    (void)l_priv;
    (void)p_bkv_dico_handle;
    if (NULL == l_ctx){
    }
    else {
        l_bkv_create.create_type=BKV_CREATE_TYPE_WORK_IN_RAM;
        if (BKV_OK != (l_ret = bkv_create(&l_bkv_create,&l_lookup_ctx.dico))){
            return(l_ret);
        }
        else if (BKV_OK != (l_ret = bkv_kv_map_open(l_lookup_ctx.dico,BKV_DICO_KEY))){
            return(l_ret);
        }
        else if (W_NO_ERROR != wg_keytree_foreach(l_ctx->head, dico_cached_elem, &l_lookup_ctx)){
        }
        else if (BKV_OK != (l_ret = bkv_kv_map_close(l_lookup_ctx.dico))){
            return(l_ret);
        }
        else if (W_NO_ERROR != wg_keytree_release(l_ctx->head)){
        }
        else {
            *p_bkv_dico_handle=l_lookup_ctx.dico;
        }
    }
    return(l_ret);
}

