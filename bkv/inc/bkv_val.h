/**
** @file bkv_val.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_VAL_H_
#define  BKV_VAL_H_

#include <stdint.h>
#include <stdbool.h>
#include <bkv_error.h>
#include <bkv_types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

typedef enum {
    BKV_VAL_TYPE_STRING,
    BKV_VAL_TYPE_INT16,
    BKV_VAL_TYPE_NUMBER,
    BKV_VAL_TYPE_FLOAT,
    BKV_VAL_TYPE_OBJECT,
    BKV_VAL_TYPE_ARRAY,
    BKV_VAL_TYPE_END_VALUE
}  bkv_val_type_t;

#define BKV_KEY_INVALID ((uint16_t)0xFFF)

typedef struct bkv_val_s bkv_val_t;
struct bkv_val_s {
    bkv_val_type_t type;
    int16_t        key;
    union{
        struct {
            uint8_t *str;    /*!< array of character not terminated by 0. */
            int len;      /*!< amount of bytes. */
        } string;
        struct {
#ifdef BKV_SUPPOPRT_C90
            long long i; /*< integer value, if representable. */
#endif
            uint16_t   int16;
            float   f;   /*< float value, if representable. */
            double  d;   /*< double value, if representable. */
            char   *r;   /*< unparsed number in string form. */
            /** Signals whether the \em i and \em d members are
             * valid. See \c YAJL_NUMBER_INT_VALID and
             * \c YAJL_NUMBER_DOUBLE_VALID. */
            unsigned int flags;
        } number;
        struct {
            uint8_t *ptr;
        } object;
        struct {
            bkv_val_t *values; /*< Array of elements. */
            int len; /*< Number of elements. */
        } array;

    }u;
    void *priv;
} ;
#define BKV_VAL_INIT {BKV_VAL_TYPE_END_VALUE, \
    BKV_KEY_INVALID, { { NULL , 0 } }, NULL}

typedef enum {
    BKV_CTX_STATE_IN_MAP,
    BKV_CTX_STATE_IN_ARRAY,
    BKV_CTX_STATE_END_VALUE
} bkv_ctx_state_t;
typedef struct {
    bkv_ctx_state_t state;
    int map_deep;
} bkv_ctx_t;
#define BKV_CTX_INIT { BKV_CTX_STATE_END_VALUE, 0 }

typedef enum {
    BKV_PARSE_ACTION_NONE           = 0,
    BKV_PARSE_ACTION_GOTO_END_MAP   = 1<<0,
    BKV_PARSE_ACTION_STOP_LOOP      = 1<<1,
    BKV_PARSE_ACTION_END_VALUE
} bkv_parse_retval_t;

typedef struct {
    bkv_parse_retval_t (*map_open)(void *p_data, uint8_t *p_ptr, bkv_key_t key);
    bkv_parse_retval_t (*map_close)(void *p_data, bkv_key_t key);
    bkv_parse_retval_t (*array_open)(void *p_data, uint8_t *p_ptr, int array_len, bkv_key_t key);
    bkv_parse_retval_t (*array_close)(void *p_data);
    bkv_parse_retval_t (*uint16)(void *p_data, uint8_t *p_ptr, bkv_key_t key, uint16_t value);
    bkv_parse_retval_t (*str)(void *p_data, uint8_t *p_ptr, bkv_key_t key, uint8_t *value, int len);
    bkv_parse_retval_t (*float_fn)(void *p_data, uint8_t *p_ptr, bkv_key_t key, float f);
    bkv_parse_retval_t (*double_fn)(void *p_data, uint8_t *p_ptr, bkv_key_t key, double f);
} bkv_val_callbacks_t;

int bkv_val_init(bkv_val_t *p_val, uint8_t *ptr);

/**
 * @brief Gets the described elements in the given valuee.
 * @param[in] v the origin value.
 * @param[in] p_keys the xpath like way to get the right value.
 * @param[out] nb_val the number of output value
 * @param[out] p_val the returned value.
 * @return BKV_OK in case of success.
 * @return BKV_INV_STATE in case the value is not closed.
 * @return BKV_INV_ARG in case of invalid argument.
 *
 * @note in case v is an array, @p_keys is not taken into account. Only the p_val are returned.
 */
int bkv_val_get2(bkv_val_t *v, const uint16_t *p_keys, int nb_val, bkv_val_t *p_val);

int bkv_val_foreach(bkv_val_t *p_in_value, bkv_val_callbacks_t *p_cbs,void *p_data);
/**
 * @brief Releases memory allocateed by the method #bkv_val_get2 in array for example.
 * @param[in] v the value to release.
 * @return BKV_OK in case of success.
 * @return BKV_INV_ARG in case of invalid argumen.
 */
bkv_error_t bkv_val_rel(bkv_val_t *v);

    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_VAL_H_  ----- */

