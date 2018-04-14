/**
** @file json_plugs.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  JSON_PLUGS_H_
#define  JSON_PLUGS_H_


#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

typedef struct {
    bool                         jump_next_map;
    bkv_val_t                    dico_val;
    void                        *priv_data;
    struct bkv_to_json_parser_s *parser;
} bkv_to_json_ctx_t;
#define BKV_TO_JSON_CTX_INIT {false, BKV_VAL_INIT, NULL, NULL}

typedef struct {
    bkv_t                          data_handle;
    bkv_t                          dico_handle;
    int                            deep;
    bool                           in_array;
    void                          *priv_data;
    struct bkv_from_json_parser_s *parser;
} bkv_from_json_ctx_t;
#define BKV_FROM_JSON_CTX_INIT {NULL, NULL , 0, false, NULL, NULL}

struct bkv_from_json_parser_s {
    int (*from_json_parse)(bkv_from_json_ctx_t *p_ctx, const uint8_t *ptr, int len);
};

struct bkv_to_json_parser_s{
    int (*to_json_init)(bkv_to_json_ctx_t *l_ctx);
    bkv_error_t (*to_json_map_open_fn)(bkv_to_json_ctx_t *l_ctx, const uint8_t *key, int keylen);
    int (*to_json_map_close_fn)(bkv_to_json_ctx_t *l_ctx);
    int (*to_json_array_open_fn)(bkv_to_json_ctx_t *l_ctx, int array_len,const uint8_t *key, int keylen);
    int (*to_json_array_close_fn)(bkv_to_json_ctx_t *l_ctx);
    int (*to_json_uint16_fn)(bkv_to_json_ctx_t *l_ctx, uint8_t *str, int strlen, uint16_t value);
    bkv_error_t (*to_json_float_fn)(bkv_to_json_ctx_t *l_ctx, uint8_t *str, int strlen, float f);
    bkv_error_t (*to_json_array_float_fn)(bkv_to_json_ctx_t *l_ctx, float f);
    int (*to_json_str_fn)(bkv_to_json_ctx_t *l_ctx, uint8_t *p_key_str, int key_strlen, uint8_t *p_value_str, int value_strlen);
    int (*to_json_finalize)(bkv_to_json_ctx_t *l_ctx, uint8_t **p_str, int *p_strlen);
};

    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef JSON_PLUGS_H_  ----- */

