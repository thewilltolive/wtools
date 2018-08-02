#include <stdio.h>
#ifdef HAS_DICO
#include "bkv_dico.h"
#endif
#include "bkv_json.h"
#include "bkv_val.h"
#include "json_plugs.h"


bkv_error_t
bkv_from_json(bkv_dico_type_t         dico_type,
              bkv_from_json_parser_t  p,
              uint8_t                *ptr,
              int                     len,
              bkv_t                  *h){
    bkv_from_json_ctx_t l_ctx=BKV_FROM_JSON_CTX_INIT;
    bkv_create_t        l_bkv_create=BKV_CREATE_INIT;
#ifdef HAS_DICO
    bkv_dico_create_t   l_bkv_dico_create=BKV_DICO_CREATE_INIT;
#endif
    bkv_error_t         l_error;

    l_bkv_create.create_type=BKV_CREATE_TYPE_WORK_IN_RAM;
    if (BKV_OK != (l_error = bkv_create(&l_bkv_create,&l_ctx.data_handle))){
        return(l_error);
    }

#ifdef HAS_DICO
    l_bkv_dico_create.type=dico_type;
    if (BKV_OK != (l_error = bkv_dico_create(&l_bkv_dico_create,&l_ctx.dico_handle))){
        return(l_error);
    }
#else
    if (BKV_OK != (l_error = bkv_create(&l_bkv_create,&l_ctx.dico_handle))){
        return(l_error);
    }
    if (BKV_OK != (l_error = bkv_kv_map_open(p_ctx->dico_handle,BKV_NO_KEY))){
        return(l_error);
    }
#endif
    if (BKV_OK != (l_error=p->from_json_parse(&l_ctx,ptr,len))){
        return(l_error);
    }
    if (NULL != l_ctx.dico_handle){
        bkv_t l_bkv_dico_handle=NULL;
#ifdef HAS_DICO
        if (BKV_OK != (l_error = bkv_dico_destroy(l_ctx.dico_handle,&l_bkv_dico_handle))){
        }
#else
        l_bkv_dico_handle = l_ctx.dico_handle;
        if (BKV_OK != (l_error = bkv_kv_map_close(l_bkv_dico_handle))){
        }
#endif
        else if (BKV_OK != (l_error = bkv_kv_key_add(l_ctx.data_handle,BKV_DICO_KEY))){
            printf("Failed to add dico key\n");
        }
        else if (BKV_OK != (l_error = bkv_append(l_ctx.data_handle,l_bkv_dico_handle))){
            printf("Failed to append dico\n");
        }
        else if (BKV_OK != (l_error = bkv_kv_map_close(l_ctx.data_handle))){
            printf("Failed to close map");
        }
#ifndef HAS_DICO
        bkv_destroy(l_bkv_dico_handle);
#endif
    }

    *h=l_ctx.data_handle;
    return(l_error);
}


static bkv_parse_retval_t
bkv_to_json_map_open(void *p_data, uint8_t *p_ptr, bkv_key_t key){
    bkv_parse_retval_t l_ret=BKV_PARSE_ACTION_NONE;
    bkv_to_json_ctx_t *l_ctx=(bkv_to_json_ctx_t*)p_data;
    bkv_key_t          l_keys[2]= { key, BKV_KEY_INVALID};
    bkv_val_t          l_val;

    (void)key;
    (void)p_ptr;
    if (NULL != l_ctx){
        if (true == l_ctx->jump_next_map){
            l_ret=BKV_PARSE_ACTION_GOTO_END_MAP;
        }
        else if (key == BKV_NO_KEY){
            /* case of the first map of a json. */
            if (BKV_OK != l_ctx->parser->to_json_map_open_fn(l_ctx,NULL,0)){
                l_ret=BKV_PARSE_ACTION_STOP_LOOP;
            }
        }
        else if (BKV_OK != bkv_val_get2(&l_ctx->dico_val,&l_keys[0],1,&l_val)){
        }
        else  if (BKV_OK != l_ctx->parser->to_json_map_open_fn(l_ctx,
                                                               l_val.u.string.str,
                                                               l_val.u.string.len)){
            l_ret=BKV_PARSE_ACTION_STOP_LOOP;
        }
    }
    return(l_ret);
}

static bkv_parse_retval_t
bkv_to_json_map_close(void *p_data, bkv_key_t key){
    bkv_parse_retval_t l_ret=BKV_PARSE_ACTION_NONE;
    bkv_to_json_ctx_t *l_ctx=(bkv_to_json_ctx_t*)p_data;
    (void)key;
    if (NULL != l_ctx){
        if (BKV_OK != l_ctx->parser->to_json_map_close_fn(l_ctx)){
            printf("Failed to close map\n");
            l_ret=BKV_PARSE_ACTION_STOP_LOOP;
        }
    }
    return(l_ret);
}

static int 
bkv_to_json_map_key(void *p_data, bkv_key_t key){
    int                       l_ret=0;
    bkv_to_json_ctx_t *l_ctx=(bkv_to_json_ctx_t*)p_data;
    (void)p_data;
    (void)key;
    if (key == BKV_DICO_KEY){
        l_ctx->jump_next_map=true;
        l_ret=BKV_PARSE_ACTION_GOTO_END_MAP;
    }
#if 0
    bkv_json_yajl_from_ctx_t *l_ctx=(bkv_json_yajl_from_ctx_t*)p_data;
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

static bool
bkv_to_json_uint16(void *p_data, uint8_t *p_ptr, bkv_key_t key, uint16_t value){
    bkv_to_json_ctx_t *l_ctx=(bkv_to_json_ctx_t*)p_data;
    bkv_key_t            l_keys[2]= { key, BKV_KEY_INVALID};
    bool                 l_continue=true;
    bkv_val_t            l_val;

    (void)p_ptr;
    if (NULL != l_ctx){
        if (BKV_OK != bkv_val_get2(&l_ctx->dico_val,&l_keys[0],1,&l_val)){
            l_continue=false;
        }
        else if (BKV_OK != l_ctx->parser->to_json_uint16_fn(l_ctx,
                                                         l_val.u.string.str,
                                                         l_val.u.string.len,
                                                         value)){
            l_continue=false;
        }
    }
    return(l_continue);
}

static bkv_parse_retval_t
bkv_to_json_str(void *p_data, uint8_t *p_ptr, bkv_key_t key, uint8_t *p_str, int len){
    bkv_to_json_ctx_t *l_ctx=(bkv_to_json_ctx_t*)p_data;
    bkv_key_t            l_keys[2]= { key, BKV_KEY_INVALID};
    int                  l_ret=0;
    bkv_val_t            l_val;

    (void)p_ptr;
    if (NULL != l_ctx){
        if (BKV_OK != bkv_val_get2(&l_ctx->dico_val,&l_keys[0],1,&l_val)){
            l_ret|=BKV_PARSE_ACTION_STOP_LOOP;
        }
        else if (BKV_OK != l_ctx->parser->to_json_str_fn(l_ctx,
                                                         l_val.u.string.str,
                                                         l_val.u.string.len,
                                                         p_str,
                                                         len)){
            l_ret|=BKV_PARSE_ACTION_STOP_LOOP;
        }
    }
    return(l_ret);
}

static bkv_parse_retval_t
bkv_to_json_array_open(void *p_data, uint8_t *p_ptr, int array_len,bkv_key_t key){
    bkv_to_json_ctx_t *l_ctx=(bkv_to_json_ctx_t*)p_data;
    int                l_ret=0;
    bkv_key_t          l_keys[2]= {key,BKV_KEY_INVALID};
    bkv_val_t          l_val;

    (void)p_ptr;
    (void)key;
    if (NULL != l_ctx){
        if (BKV_OK != bkv_val_get2(&l_ctx->dico_val,&l_keys[0],1,&l_val)){
        }
        else if (BKV_OK != l_ctx->parser->to_json_array_open_fn(l_ctx,
                                                                array_len,
                                                                l_val.u.string.str,
                                                                l_val.u.string.len)){
            l_ret|=BKV_PARSE_ACTION_STOP_LOOP;
        }
    }
    return(l_ret);
}

static bkv_parse_retval_t
bkv_to_json_array_close(void *p_data){
    bkv_to_json_ctx_t *l_ctx=(bkv_to_json_ctx_t*)p_data;
    int                  l_ret=0;

    if (NULL != l_ctx){
        if (BKV_OK != l_ctx->parser->to_json_array_close_fn(l_ctx)){
            l_ret|=BKV_PARSE_ACTION_STOP_LOOP;
        }
    }
    return(l_ret);
}

static bkv_parse_retval_t
bkv_to_json_float(void *p_data, uint8_t *p_ptr, bkv_key_t key, float f){
    bkv_to_json_ctx_t *l_ctx=(bkv_to_json_ctx_t*)p_data;
    bkv_key_t          l_keys[2]= { key, BKV_KEY_INVALID};
    int                l_ret=0;
    bkv_val_t          l_val;

    (void)p_ptr;
    if (NULL != l_ctx){
        if (BKV_ARRAY_KEY == key){
            if (BKV_OK != l_ctx->parser->to_json_array_float_fn(l_ctx,
                                                                f)){
                l_ret|=BKV_PARSE_ACTION_STOP_LOOP;
            }
        }
        else {
            if (BKV_OK != bkv_val_get2(&l_ctx->dico_val,&l_keys[0],1,&l_val)){
            }
            else if (BKV_OK != l_ctx->parser->to_json_float_fn(l_ctx,
                                                               l_val.u.string.str,
                                                               l_val.u.string.len,
                                                               f)){
                l_ret|=BKV_PARSE_ACTION_STOP_LOOP;
            }
        }
    }
    return(l_ret);
}


static bkv_val_callbacks_t s_bkv_callbacks ={
    bkv_to_json_map_open,
    bkv_to_json_map_close,
    bkv_to_json_array_open,
    bkv_to_json_array_close,
    bkv_to_json_map_key,
    bkv_to_json_uint16,
    bkv_to_json_str,
    bkv_to_json_float
};


int bkv_to_json(bkv_to_json_parser_t  p,
                bkv_t                 h,
                uint8_t             **p_ptr,
                int                  *p_len){
    int                      l_ret=BKV_KO,l_ptrlen=0;
    uint8_t                 *l_ptr;
    bkv_val_t                l_bkv_head;
    bkv_to_json_ctx_t        l_ctx=BKV_TO_JSON_CTX_INIT;
    bkv_key_t                l_dico_key[2]={BKV_DICO_KEY,BKV_KEY_INVALID};

    if (-1 == (l_ret = p->to_json_init(&l_ctx))){
    } else {
        l_ctx.parser=p;
        bkv_get_head(h,&l_ptr,&l_ptrlen);
        bkv_val_init(&l_bkv_head,l_ptr);
        if (BKV_OK != (l_ret=bkv_val_get2(&l_bkv_head, &l_dico_key[0], 1, &l_ctx.dico_val))){
            printf("Failed to find dico key\n");
        }
        else if (BKV_OK != (l_ret=bkv_val_foreach(&l_bkv_head,&s_bkv_callbacks,&l_ctx))){
        }
        else if (BKV_OK != (l_ret=p->to_json_finalize(&l_ctx,p_ptr,p_len))){
        }
        else {
            /* ok. */
        }
    }

    return (l_ret);
}

