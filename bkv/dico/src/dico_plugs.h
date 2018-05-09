/*
*/

/**
** @file dico_plugs.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  DICO_PLUGS_H_
#define  DICO_PLUGS_H_

#include "bkv.h"
#include "keytree.h"
#include "bkv_dico.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

typedef int (*dico_create_t)(bkv_dico_create_t *p_create, void **l_priv);
typedef int (*dico_key_add_t)(void *l_priv, bkv_key_t key, const uint8_t *keystr, int keystrlen);
typedef int (*dico_destroy_t)(void *l_priv, bkv_t *p_bkv_dico_handle);
typedef struct {
    dico_create_t  create;
    dico_key_add_t key_add;
    dico_destroy_t destroy;
} dico_plug_t;

typedef struct {
    dico_plug_t *plug;
    union {
        keytree_head_t h;
        bkv_t          dico;
    } u;
    void *priv_handle;
}dico_ctx_t;


extern dico_plug_t s_dico_direct_plug;
extern dico_plug_t s_dico_cached_plug;

    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef DICO_PLUGS_H_  ----- */

