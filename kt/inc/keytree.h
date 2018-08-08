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
#define WG_KEY_INIT { NULL, 0 }

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
    void          *udata; /*!< Private user data linked to the element.*/
} keytree_elem_t;
#define KEYTREE_ELEM_INIT { { NULL, 0 }, 0} 
#define KEYTREE_ELEM_VALUE(a,u) {{(uint8_t*)a,strlen(a)},(void*)u}

/*!
 * @brief keytree list.
 */
typedef struct {
    keytree_elem_t *elems;
    int             nb_elems;
} keytree_list_t;
#define KEYTREE_LIST_INIT { NULL, 0 }

/**
 ** @brief Defines a key structure.
 **/
struct wg_key_s
{
    /*uint8_t key; */ /*!< key value. */
    bool    end_of_word;
    union
    {
        keytree_head_t head;
    } u;
    keytree_data_t udata;
};

/**
 * @brief Callback used to return a dictionnary element. 
 * @param[in] p_key the current key
 * @param[in] p_str the current string.
 * @param[in] strlen the current string length
 * @return #w_error_t
 */
typedef w_error_t (*wg_keytree_fn_t)(wg_key_t      *p_key,
                                     const uint8_t *p_str,
                                     int            strlen,
                                     void          *p_data);

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

/**
 * @brief Adds a key to the tree.
 * @param[in] head the tree head.
 * @param[in] e the element to add
 * @param[out] udata in case the added element already exists, udata is the private data of the existing element.
 * @return W_NO_ERROR in case of success.
 * @return W_E_EXISTS in case the given element string already exists. In this case, udata is filled with the private data.
 * @return W_E_BAD_PARAMETER in case of invalid parameter.
 */
w_error_t wg_keytree_add(keytree_head_t *head,
                         keytree_elem_t *e,
                         void           **udata);

/**
 ** @brief Compares a string to the keytree.
 ** @retval W_NO_ERROR        if the string str is found in the key tree.
 ** @retval W_E_BAD_PARAMETER if the given string is not found in the key tree.
 **/
w_error_t wg_keytree_strcmp(keytree_head_t  *key_head,
                            const char     *str);

/**
 * @brief Loops on each dictionnary element and provide each element in the #wg_keytree_fn_t callback.
 * @param key_head
 * @param fn
 * @param p_data
 * @return 
 */
w_error_t wg_keytree_foreach(keytree_head_t  *key_head,
                             wg_keytree_fn_t  fn,
                             void            *p_data);
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
                           void           *udata);

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

