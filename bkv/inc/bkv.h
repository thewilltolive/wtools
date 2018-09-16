/*
*/

/**
** @file bkv.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_H_
#define  BKV_H_

#include <stdint.h>
#include <bkv_types.h>
#include "bkv_error.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bkv_s *bkv_t;
/**
 * @brief Specific key numbers.
 */
typedef enum {
    BKV_NO_KEY   =0x0,  /* No key means only for the first map. */
    BKV_ARRAY_KEY=0xFFD,
    BKV_DICO_KEY =0xFFE
} bkv_key_spec_t;

typedef struct {
    void (*p)(char *fmt, ...);
} bkv_init_t;
#define BKV_INIT { NULL }

/**
 * @brief Defines bkv creation types.
 */
typedef enum {
    BKV_CREATE_TYPE_WORK_IN_RAM                 =1<<0,
    BKV_CREATE_TYPE_OPEN_FILE_CREAT_AND_WRITE   =1<<1,
    BKV_CREATE_TYPE_OPEN_FILE_READ_ONLY         =1<<2,
    BKV_CREATE_TYPE_INPUT_BUFFER                =1<<3,
    BKV_CREATE_TYPE_END_VALUE
} bkv_create_type_t;

/**
 * @brief Defines bkv optimisation type.
 */
typedef enum {
    BKV_OPTIM_TYPE_SIZE                         =1<<0,
    BKV_OPTIM_TYPE_GENERATION                   =1<<1,
    BKV_OPTIM_TYPE_END_VALUE
} bkv_optim_type_t;

typedef struct {
    bkv_create_type_t  create_type;      /*!< create type.*/
    bkv_optim_type_t   optim_type;       /*!< Optimisation type. Not implemented. */
    const char        *filename;         /*!< storage file name in case create_type is 
                                       #BKV_CREATE_TYPE_OPEN_FILE_CREAT_AND_WRITE or #BKV_CREATE_TYPE_OPEN_FILE_READ_ONLY. */
    uint8_t           *input_ptr;
    int                mode;             /*!< Mode of a created file in case create_type is #BKV_CREATE_TYPE_OPEN_FILE_CREAT_AND_WRITE */
    int                mem_chunk_size;   /*!< If not -1 and inferior to 1<<12, allocated chunk size using malloc().*/
    int                max_mem_size;     /*!< Max memory usage when adding keys. */
} bkv_create_t;
#define BKV_CREATE_INIT  { BKV_CREATE_TYPE_END_VALUE, \
    BKV_OPTIM_TYPE_END_VALUE,\
    NULL, NULL, 0 , -1 , -1}

/**
 * @brief Initializes the bkv library.
 * @param[in] i the initialisation parameters.
 * @return BKV_OK in case of success.
 * @return BKV_KO in case of failure.
 * @return BKV_INV_STATE in case the @bkv_init method has already been called.
 */
extern int bkv_init(bkv_init_t *i);

/**
 * @brief Terminates the bkv library.
 * @return BKV_OK in case of success 
 * @return BKV_INV_STATE in case the @bkv_init method has not been called.
 */
extern int bkv_term(void);

/**
 * @brief Creates a bkv handle.
 * @param[in] p_create the bkv creation parameters.
 * @param[out] p_handle the returned handle.
 * @return BKV_OK in case of success.
 * @return BKV_INV_ARG in case of failure.
 */
extern int bkv_create(bkv_create_t *p_create, bkv_t *p_handle);

/**
 * @brief Returns a memory pointer of the head map of the given bkv.
 *
 * @param[in] h the bkv handle.
 * @param[out] p_head the returned pointer on the memory zone of the bkv.
 * @param[out] p_len the length of the memory zone.
 *
 * @return BKV_OK in case of success. 
 * @return BKV_INV_ARG in case of failure.
 */
extern int bkv_get_head(bkv_t h, uint8_t **p_head, int *p_len);

/*!
 * @brief Synchonizes the data created in the bkv handle to a file (in case it is open with Read-Write file access).
 * @param[in] handle The handle to flush.
 * @return 0 in case of success.
 * @return -1 in case of io failure (from posix)
 * @return -2 in case of erroneous handle creation flag.
*/
extern int bkv_sync(bkv_t handle);

/*!
 * @brief Destroys the given bkt_t handle.
 * @param[in] handle the handle to destroy.
 * @return -1 in case of error.
 * @return 0 in case of success.
 * @note This function destroys the handle and the referenced memory without any saving operation.
 *       User shall call #bkv_buffer_get() to acquired the memory or #bkv_buffer_sync() to synchronize
 *       the data to a file in case the kbs handle has been opened with a Read-Write file pointer.
 */
extern int bkv_destroy(bkv_t handle);

/**
 * @brief Adds unsigned 16-bits length value to the bkv.
 * @param[in] h Handle of the bkv.
 * @param[in] key Key of the value.
 * @param[in] v Value to add.
 * @return BKV_OK in case of success
 * @return BKV_INV_ARG in case of invalid argument.
 */
extern int bkv_kv_u16_add(bkv_t h,uint16_t key, uint16_t v);

/**
 * @brief Adds unsigned 32-bits length value to the bkv.
 * @param[in] h Handle of the bkv.
 * @param[in] key Key of the value.
 * @param[in] v Value to add.
 * @return BKV_OK in case of success
 * @return BKV_INV_ARG in case of invalid argument.
 */
extern int bkv_kv_u32_add(bkv_t h,uint32_t key, uint32_t v);

extern int bkv_kv_float_add(bkv_t h,uint16_t key, float v);
extern int bkv_kv_double_add(bkv_t h,uint16_t key, double v);
extern int bkv_size(bkv_t h);

/**
 * @brief Adds a string to the bkv.
 * @param h bkv handle.
 * @param key the key of the string.
 * @param str the string not necessarly NULL-terminated.
 * @param len length of the string.
 * @return BKV_OK in case of success.
 */
extern int bkv_kv_str_add(bkv_t h, bkv_key_t key, const uint8_t *str, int len);
extern int bkv_append(bkv_t h, bkv_t a);
extern int bkv_kv_map_open(bkv_t h, bkv_key_t key);
extern int bkv_kv_map_close(bkv_t h);
extern int bkv_kv_array_open(bkv_t h, bkv_key_t key);
extern int bkv_kv_array_close(bkv_t h);
extern int bkv_kv_array_float_add(bkv_t h, float f);

/**
 * @brief Adds a string to the array bkv.
 * @param h bkv handle.
 * @param str the string not necessarly NULL-terminated.
 * @param len length of the string.
 * @return BKV_OK in case of success.
 */
extern int bkv_kv_array_str_add(bkv_t h, const uint8_t *str, int len);
extern int bkv_kv_array_u16_add(bkv_t h, uint16_t u);
#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_H_  ----- */

