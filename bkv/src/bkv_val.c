
#include <stdbool.h>
#include <stdio.h>
#include "bkv.h"
#include "bkv_val.h"
#include "bkv_byteorder.h"
#include "bkv_header.h"


int bkv_val_init(bkv_val_t *p_val, uint8_t *p_ptr){
    int      l_ret=-1;
    int      l_type;

    l_type = (p_ptr[0] >> 4) & 0xF;
    p_val->key = ((p_ptr[0] & 0xF) << 8) + ( p_ptr[1] & 0xFF);
    switch(l_type){
    case HDR_TYPE_MAP_OPEN:
        p_val->type=BKV_VAL_TYPE_OBJECT;
        l_ret=0;
        break;
    case HDR_TYPE_MAP_CLOSE:
        break;
    case HDR_TYPE_ARRAY_OPEN:
        break;
    case HDR_TYPE_ARRAY_CLOSE:
        break;
    case HDR_TYPE_INT16:
        p_val->type=BKV_VAL_TYPE_NUMBER;
        le16_to_cpu(&p_ptr[HDR_SIZE],&p_val->u.number.int16,NULL);
        l_ret=0;
        break;
    case HDR_TYPE_INT32:
        break;
    case HDR_TYPE_INT64:
        break;
    case HDR_TYPE_STRING:
        break;
    case HDR_TYPE_FLOAT:
        break;
    case HDR_TYPE_DOUBLE:
        break;
    default:
        break;

    }
    p_val->priv=p_ptr;
    return(l_ret);
}

static int 
val_foreach(bkv_val_t           *v,
            bkv_val_callbacks_t *p_callbacks,
            void                *p_data){
    int                l_ret=0, l_strlen;
    uint8_t           *l_base=v->priv,*l_ptr;
    int16_t            l_key;
    uint16_t           l_value,l_map_length,l_array_nb_elems;
    header_type_t      l_hdr_type;
    bkv_ctx_t          l_ctx= BKV_CTX_INIT;
    bool               l_continue=true;
    float              l_float;

    l_ptr=l_base;

    do {
        l_hdr_type = (l_ptr[0] >> 4) & 0xF;
        l_key = ((l_ptr[0] & 0xF) << 8) + ( l_ptr[1] & 0xFF);

        switch(l_hdr_type){
        case HDR_TYPE_MAP_OPEN:
            if (0 != (l_ret & BKV_PARSE_ACTION_GOTO_END_MAP)){
                l_map_length=((l_ptr[2] & 0xFF) << 8) + ( l_ptr[3] & 0xFF);
                if (0 == l_map_length){
                    printf("Invalid bkv: map not closed\n");
                    return(BKV_INV_ARG);
                }
                l_ptr+=l_map_length;
            }
            else {
                l_ret=p_callbacks->map_open(p_data,l_ptr,l_key);
                if (0 != (l_ret & BKV_PARSE_ACTION_GOTO_END_MAP)){
                    l_map_length=((l_ptr[2] & 0xFF) << 8) + ( l_ptr[3] & 0xFF);
                    if (0 == l_map_length){
                        printf("Invalid bkv: map not closed\n");
                        return(BKV_INV_ARG);
                    }
                    l_ptr+=l_map_length;
                }
                else {
                    l_ctx.map_deep++;
                    l_ptr+=HDR_SIZE;
                    l_ptr+=MAP_OPEN_VALUE_SIZE;
                }
            }
            l_ctx.state=BKV_CTX_STATE_IN_MAP;
            break;
        case HDR_TYPE_MAP_CLOSE:
            l_ptr+=HDR_SIZE+MAP_CLOSE_VALUE_SIZE;
            l_ctx.map_deep--;
            l_ret=p_callbacks->map_close(p_data,l_key);
            if (0 == l_ctx.map_deep){
                l_ret=BKV_PARSE_ACTION_STOP_LOOP;
            }
            break;
        case HDR_TYPE_ARRAY_OPEN:
            l_ptr+=HDR_SIZE;
            l_ptr+=2;
            le16_to_cpu(l_ptr,&l_array_nb_elems,NULL);
            l_ret=p_callbacks->array_open(p_data, l_ptr, l_array_nb_elems, l_key);
            l_ptr+=2;
            l_ctx.state=BKV_CTX_STATE_IN_ARRAY;
            break;
        case HDR_TYPE_ARRAY_CLOSE:
            l_ptr+=HDR_SIZE;
            l_ret=p_callbacks->array_close(p_data);
            l_ptr+=ARRAY_CLOSE_VALUE_SIZE;
            break;
        case HDR_TYPE_INT16:
            l_ptr+=HDR_SIZE;
            le16_to_cpu(l_ptr,&l_value,NULL);
            l_continue=p_callbacks->uint16(p_data,l_ptr,l_key, l_value);
            l_ptr+=sizeof(int16_t);
            break;
        case HDR_TYPE_KEY:
            l_ret=p_callbacks->key(p_data,l_key);
            l_ptr+=HDR_SIZE;
            break;
        case HDR_TYPE_INT32:
            break;
        case HDR_TYPE_INT64:
            break;
        case HDR_TYPE_STRING:
            l_ptr+=HDR_SIZE;
            l_strlen = ((l_ptr[0] & 0xFF) << 8) + ( l_ptr[1] & 0xFF);
            l_ptr+=2;
            l_ret=p_callbacks->str(p_data,l_ptr,l_key, l_ptr, l_strlen);
            l_ptr+=l_strlen;
            break;
        case HDR_TYPE_FLOAT:
            l_ptr+=HDR_SIZE;
            lef_to_cpu(l_ptr,&l_float,NULL);
            l_ret=p_callbacks->float_fn(p_data,l_ptr,l_key, l_float);
            l_ptr+=sizeof(float);
            break;
        case HDR_TYPE_DOUBLE:
            break;
        default:
            l_ret=-1;
            break;

        }
    } while((0 == (l_ret & BKV_PARSE_ACTION_STOP_LOOP)) && 
            ((int)((int)l_ptr - (int)l_base) > 0) && 
            (true==l_continue));

    return(BKV_OK);
}

typedef struct {
    bkv_val_t       *origin;
    const bkv_key_t *keys;
    int              nb_val;
    bkv_val_t       *val;
    int              val_offset;
    bool             got_it;
    int              key_offset;
    bool             val_is_set;
} bkv_val_get_ctx_t;


static bkv_parse_retval_t  bkv_val_get_map_open(void *p_data, uint8_t *p_ptr, bkv_key_t key){
    bkv_parse_retval_t        l_ret=0;
    bkv_val_get_ctx_t        *l_ctx=p_data;
    (void)p_data;
    (void)key;
    if (NULL == l_ctx){
        return(-1);
    }
    if (true == l_ctx->got_it){
        l_ctx->val->type=BKV_VAL_TYPE_OBJECT;
        l_ctx->val->u.object.ptr=p_ptr;
        l_ctx->val->priv=p_ptr;
        l_ctx->val_is_set=true;
        l_ret=BKV_PARSE_ACTION_STOP_LOOP;
    }

    return(l_ret);
}
static bkv_parse_retval_t
bkv_val_get_map_close(void *p_data, bkv_key_t key){
    bool l_ret=BKV_PARSE_ACTION_NONE;
    (void)p_data;
    (void)key;
    return(l_ret);
}

static int 
bkv_val_get_map_key(void *p_data, bkv_key_t key){
    int l_ret=0;
    bkv_val_get_ctx_t        *l_ctx=p_data;
    (void)p_data;
    (void)key;
    if (key == l_ctx->keys[l_ctx->key_offset]){
        if (l_ctx->keys[l_ctx->key_offset+1] == BKV_KEY_INVALID){
            l_ctx->got_it=true;
        }
    }
    return(l_ret);
}

static bool
bkv_val_get_uint16(void *p_data, uint8_t *p_ptr, bkv_key_t key, uint16_t value){
    bkv_val_get_ctx_t        *l_ctx=p_data;
    bool                      l_continue=true;
    (void)p_data;
    (void)p_ptr;
    (void)value;
    (void)key;
    if (key == l_ctx->keys[l_ctx->key_offset]){
        if (l_ctx->keys[l_ctx->key_offset+1] == BKV_KEY_INVALID){
            l_ctx->val->type=BKV_VAL_TYPE_INT16;
            l_ctx->val->u.number.int16=value;
            l_ctx->val_is_set=true;
            l_continue=false;
        }
    }
    return(l_continue);
}

static bkv_parse_retval_t
bkv_val_get_str(void *p_data, uint8_t *p_ptr, bkv_key_t key, uint8_t *str, int len){
    int                       l_ret=0;
    bkv_val_get_ctx_t        *l_ctx=p_data;
    (void)p_data;
    (void)p_ptr;
    (void)key;
    if (key == l_ctx->keys[l_ctx->key_offset]){
        if (l_ctx->keys[l_ctx->key_offset+1] == BKV_KEY_INVALID){
            l_ctx->val->type=BKV_VAL_TYPE_STRING;
            l_ctx->val->u.string.str=str;
            l_ctx->val->u.string.len=len;
            l_ctx->val_is_set=true;
            l_ret=BKV_PARSE_ACTION_STOP_LOOP;
        }
    }
    return(l_ret);
}

static bkv_parse_retval_t
bkv_val_get_float(void *p_data, uint8_t *p_ptr, bkv_key_t key, float f){
    bkv_val_get_ctx_t        *l_ctx=p_data;
    bkv_parse_retval_t        l_ret=0;
    (void)p_data;
    (void)p_ptr;
    if ((key == l_ctx->keys[l_ctx->key_offset]) || 
        (true == l_ctx->got_it)){
        if (l_ctx->keys[l_ctx->key_offset+1] == BKV_KEY_INVALID){
            l_ctx->val[l_ctx->val_offset].type=BKV_VAL_TYPE_FLOAT;
            l_ctx->val[l_ctx->val_offset].u.number.f=f;
            l_ctx->val_is_set=true;
            if ((l_ctx->val[0].type==BKV_VAL_TYPE_ARRAY)&&
                (l_ctx->val_offset < l_ctx->val[0].u.array.len)&&
                (l_ctx->val_offset < l_ctx->nb_val)){
            }
            else {
                l_ret=BKV_PARSE_ACTION_STOP_LOOP;
            }
            l_ctx->val_offset++;
        }
    }
    return(l_ret);
}

static bkv_parse_retval_t
bkv_val_get_array_open(void *p_data, uint8_t *p_ptr, int array_len, bkv_key_t key){
    bkv_val_get_ctx_t  *l_ctx=p_data;
    int                 l_ret=0;
    (void)p_ptr;
    if (key == l_ctx->keys[l_ctx->key_offset]){
        if (l_ctx->keys[l_ctx->key_offset+1] == BKV_KEY_INVALID){
            l_ctx->val->type=BKV_VAL_TYPE_ARRAY;
            l_ctx->got_it=true;
            l_ctx->val->u.array.len=array_len;
            l_ctx->val_offset++;
        }
    }
    return(l_ret);
}

static bkv_parse_retval_t 
bkv_val_get_array_close(void *p_data){
    int l_ret=0;
    (void)p_data;
    return(l_ret);
}

static bkv_val_callbacks_t s_val_get_callbacks = {
    bkv_val_get_map_open,
    bkv_val_get_map_close,
    bkv_val_get_array_open,
    bkv_val_get_array_close,
    bkv_val_get_map_key,
    bkv_val_get_uint16,
    bkv_val_get_str,
    bkv_val_get_float
};

int bkv_val_get2(bkv_val_t *v, 
                 const uint16_t *p_keys,
                 int nb_val,
                 bkv_val_t *p_val){
    int               l_ret=1;
    bkv_val_get_ctx_t l_ctx = { v, p_keys , nb_val, p_val, 0, false, 0, false};

    l_ret = val_foreach(v,&s_val_get_callbacks,&l_ctx);

    if (true == l_ctx.val_is_set){
        l_ret = BKV_OK;
    }

    return(l_ret);
}

int bkv_val_foreach(bkv_val_t           *p_val,
                    bkv_val_callbacks_t *p_callbacks,
                    void                *p_data){
    return(val_foreach(p_val,p_callbacks,p_data));
}
