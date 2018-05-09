/*
*/

/**
** @file bkv_dico.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_DICO_H_
#define  BKV_DICO_H_

#include "bkv.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

typedef void *bkv_dico_t;
typedef enum {
    BKV_DICO_TYPE_DIRECT,
    BKV_DICO_TYPE_CACHED,
    BKV_DICO_TYPE_END_VALUE
} bkv_dico_type_t;

typedef struct {
    bkv_dico_type_t type;
    bkv_create_t    bkv_create;
} bkv_dico_create_t;
#define BKV_DICO_CREATE_INIT { BKV_DICO_TYPE_END_VALUE, BKV_CREATE_INIT } 

int bkv_dico_create(bkv_dico_create_t *p_dico_create, bkv_dico_t *p_dico_handle);
int bkv_dico_key_add(bkv_dico_t dico_handle,bkv_key_t key_index, const uint8_t *keystr, int keystrlen);
int bkv_dico_destroy(bkv_dico_t dico_handle, bkv_t *p_bkv_dico_handle);


    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_DICO_H_  ----- */

