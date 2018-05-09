#include "keytree.h"
#include "dico_plugs.h"


static bkv_error_t dico_cached_create(bkv_dico_create_t *p_create, void **l_priv);
static bkv_error_t dico_cached_key_add(void *l_priv, bkv_key_t key, const uint8_t *keystr, int keystrlen);
static bkv_error_t dico_cached_destroy(void *l_priv, bkv_t *p_bkv_dico_handle);

typedef struct {
    keytree_head_t head;
} dico_cached_ctx_t;

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
static bkv_error_t dico_cached_destroy(void *l_priv, bkv_t *p_bkv_dico_handle){
    (void)l_priv;
    (void)p_bkv_dico_handle;
    return(BKV_OK);
}

