#ifndef  WG_KEYTREE_H_
#define  WG_KEYTREE_H_

#include <stdint.h>
#include <stdbool.h>
#include "w_errors.h"
#include "word.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 ** @defgroup WG Word Key
 */

/**
 ** @ingroup WG
 ** @defgroup WG_KT Key Tree.
 ** @{
 ** @brief
 ** The aim of the key tree component is to create a tree of key representing the list
 ** of word that must be checked.
 **
 ** @note This API is not thread-safe.
 */



/**
 ** @brief Defines a key.
 **/
typedef struct wg_key_s wg_key_t;

/**
 ** @brief Defines a key head.
 **/
typedef struct wg_key_head_s keytree_head_t;

/*!
 * @brief Defines keytree data.
 */
typedef void *keytree_data_t;

/**
 ** @brief Defines a key list.
 **/
typedef struct 
{
    wg_key_t **keys;
    uint32_t   nb_keys;

} wg_key_list_t;

/**
 ** @brief Defines a alphanumeric key head of list.
 **/
struct wg_key_head_s
{
    wg_key_list_t lower_case_char;
    wg_key_list_t upper_case_char;
    wg_key_list_t number_char;
    wg_key_list_t other_char;

};

typedef struct {
    word_t         w;
    int d;
} keytree_elem_t;

/*!
 * @brief keytree list.
 */
typedef struct {
    keytree_elem_t *elems;
    int             nb_elems;
} keytree_list_t;

/**
 ** @brief Defines a key structure.
 **/
struct wg_key_s
{
    uint8_t key;  /*!< key value. */
    bool    end_of_word;
    union
    {
        keytree_head_t head;
    } u;
    keytree_data_t udata;
};

/**
 ** @brief Generates a keytree from a word list.
 ** @param[in] wordlist word list to analyze.
 ** @param[out] key_head head of the returned tree.
 ** @retval W_E_OPERATION_FAILED if the dist file cannot be opened.
 ** @retval W_E_BAD_PARAMETER    if a parameter is wrong.
 ** @retval W_NO_ERROR           in case of success.
 **/
w_error_t wg_keytree_create(keytree_list_t  *wordlist, 
                            keytree_head_t **key_head);

w_error_t wg_keytree_add(keytree_head_t *head,
                         keytree_elem_t *e);

/**
 ** @brief Compares a string to the keytree.
 ** @retval W_NO_ERROR        if the string str is found in the key tree.
 ** @retval W_E_BAD_PARAMETER if the given string is not found in the key tree.
 **/
w_error_t wg_keytree_strcmp(keytree_head_t  *key_head,
                            const char     *str);

int wg_keytree_strcmp_isalnum(keytree_head_t  *key_head,
                              const char      *str);

/**
 ** @brief Compares a string to the keytree.
 ** @retval W_NO_ERROR        if the string str is found in the key tree.
 ** @retval W_E_BAD_PARAMETER if the given string is not found in the key tree.
 **/
w_error_t wg_keytree_keyget(keytree_head_t  *key_head,
                            const char      *str,
                            keytree_elem_t **elem);


/**
 ** @brief Releases a key tree.
 ** @param[in] key_head head of the key.
 **/
w_error_t wg_keytree_release(keytree_head_t *key_head);

/**
 ** @brief Initializes a key.
 **/
w_error_t keytree_key_init(keytree_elem_t *k,
                           const char     *str,
                           int             d);

/*!
 * @brief Uninitializes a key.
 */
w_error_t keytree_key_uninit(keytree_elem_t *k);

/**
 *@}
 * End of group
 */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef WG_KEYTREE_H_  ----- */
