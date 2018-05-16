#include <stdio.h>
#include "keytree.h"
#include "dico_plugs.h"


static bkv_error_t dico_cached_create(bkv_dico_create_t *p_create, void **l_priv);
static bkv_error_t dico_cached_key_add(void *l_priv, bkv_key_t key, const uint8_t *keystr, int keystrlen);
static bkv_error_t dico_cached_destroy(void *l_priv, bkv_t *p_bkv_dico_handle);

typedef struct {
    keytree_head_t *head;
} dico_cached_ctx_t;

typedef struct {
    bkv_t dico;
} dico_cached_lookup_ctx_t;

dico_plug_t s_dico_cached_plug={
    dico_cached_create,
    dico_cached_key_add,
    dico_cached_destroy
} ;


static bkv_error_t dico_cached_create(bkv_dico_create_t *p_create, void **l_priv){
    (void)p_create;
    (void)l_priv;
    return(BKV_OK);
}

static bkv_error_t dico_cached_key_add(void *l_priv, bkv_key_t key, const uint8_t *keystr, int keystrlen){
    (void)l_priv;
    (void)key;
    (void)keystr;
    (void)keystrlen;
    return(BKV_OK);
}

static w_error_t dico_cached_elem(keytree_elem_t *elem, void *p_data){
    w_error_t l_ret=W_E_BAD_PARAMETER;
    dico_cached_lookup_ctx_t *l_lookup_ctx=(dico_cached_lookup_ctx_t*)p_data;

    if (BKV_OK != bkv_kv_str_add(l_lookup_ctx->dico,elem->d,elem->w.str,elem->w.len)){
        printf("Failed to add a string to cached dico\n");
    }
    else {
        l_ret=W_NO_ERROR;
    }

    return(l_ret);
}

static bkv_error_t dico_cached_destroy(void *l_priv, bkv_t *p_bkv_dico_handle){
    bkv_error_t               l_ret=BKV_INV_ARG;
    dico_cached_ctx_t        *l_ctx=(dico_cached_ctx_t*)l_priv;
    dico_cached_lookup_ctx_t  l_lookup_ctx;
    (void)l_priv;
    (void)p_bkv_dico_handle;
    if (NULL == l_ctx){
    }
    else {
        if (W_NO_ERROR != wg_keytree_foreach(l_ctx->head, dico_cached_elem, &l_lookup_ctx)){
        }
        else if (W_NO_ERROR != wg_keytree_destroy(l_ctx->head)){
        }
    }
    return(l_ret);
}

