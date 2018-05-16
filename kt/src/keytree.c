#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "w_errors.h"
#include "keytree.h"

static uint8_t word_type(const char c)
{
    if isdigit(c)
        return(1);
    if (isalpha(c) != 0)
    {
        if isupper(c)
            return(2);
        if islower(c)
            return(3);
    }
    return(0);
}

static w_error_t wg_key_release(keytree_head_t *key_head)
{
    int         l_i;
    wg_key_t  **l_keys;
    if (key_head->number_char.keys != NULL)
    {
        l_keys=key_head->number_char.keys;
        for (l_i=0;l_i<10;l_i++){
            if (NULL != l_keys[l_i]){
                wg_key_release(&l_keys[l_i]->u.head);
                free(l_keys[l_i]);
            }
        }
        free(l_keys);
    }
    if (key_head->upper_case_char.keys != NULL)
    {
        l_keys=key_head->upper_case_char.keys;
        for (l_i=0;l_i<26;l_i++){
            if (NULL != l_keys[l_i]){
                wg_key_release(&l_keys[l_i]->u.head);
                free(l_keys[l_i]);
            }
        }
        free(l_keys);
    }
    if (key_head->lower_case_char.keys != NULL)
    {
        l_keys=key_head->lower_case_char.keys;
        for (l_i=0;l_i<26;l_i++){
            if (NULL != l_keys[l_i]){
                wg_key_release(&l_keys[l_i]->u.head);
                free(l_keys[l_i]);
            }
        }
        free(l_keys);
    }
    if (key_head->other_char.keys != NULL)
    {
        l_keys=key_head->other_char.keys;
        for (l_i=0;l_i<26;l_i++){
            if (NULL != l_keys[l_i]){
                wg_key_release(&l_keys[l_i]->u.head);
                free(l_keys[l_i]);
            }
        }
        free(l_keys);
    }

    return(W_NO_ERROR);
}

/**
 ** @brief Generates a key tree from a word list. 
 **/
static w_error_t parse_file_create_key(keytree_elem_t  *elem, 
                                       keytree_head_t  *key_head)
{
    w_error_t              l_error;
    word_t                *word=&elem->w;
    const uint8_t         *l_str=word->str;
    int                    l_iter;
    keytree_head_t        *l_key_head = key_head;
    wg_key_t              *l_key;

    for (l_iter = 0; l_iter < (int) word->len; l_iter ++)
    {
        switch(word_type(l_str[l_iter]))
        {
            case 1: /* alpha num */
                if (l_key_head->number_char.keys == NULL)
                {
                    l_key_head->number_char.keys = malloc(sizeof(wg_key_t*)*10);
                    if (NULL == l_key_head->number_char.keys)
                    {
                        l_error = W_E_NO_MEMORY;
                        goto error;
                    }
                    memset(l_key_head->number_char.keys,0,sizeof(wg_key_t*)*10);
                }
                l_key = l_key_head->number_char.keys[l_str[l_iter] - '0'];
                if (l_key == NULL)
                {
                    l_key = malloc(sizeof(wg_key_t));
                    if (NULL == l_key)
                    {
                        l_error = W_E_NO_MEMORY;
                        goto error;
                    }
                    memset(l_key,0,sizeof(*l_key));
                    l_key_head->number_char.keys[l_str[l_iter] - '0'] = l_key;
                }
                break;
            case 2: /* upper case */
                if (l_key_head->upper_case_char.keys == NULL)
                {
                    l_key_head->upper_case_char.keys = malloc(sizeof(wg_key_t*)*26);
                    if (NULL == l_key_head->upper_case_char.keys)
                    {
                        l_error = W_E_NO_MEMORY;
                        goto error;
                    }
                    memset(l_key_head->upper_case_char.keys,0,sizeof(wg_key_t*)*26);
                }
                l_key = l_key_head->upper_case_char.keys[l_str[l_iter] - 'A'];
                if (l_key == NULL)
                {
                    l_key = malloc(sizeof(wg_key_t));
                    if (NULL == l_key)
                    {
                        l_error = W_E_NO_MEMORY;
                        goto error;
                    }
                    memset(l_key,0,sizeof(*l_key));
                    l_key_head->upper_case_char.keys[l_str[l_iter] - 'A'] = l_key;
                }
                break;
            case 3: /* lower case */
                if (l_key_head->lower_case_char.keys == NULL)
                {
                    l_key_head->lower_case_char.keys = malloc(sizeof(wg_key_t*)*26);
                    if (NULL == l_key_head->lower_case_char.keys)
                    {
                        l_error = W_E_NO_MEMORY;
                        goto error;
                    }
                    memset(l_key_head->lower_case_char.keys,0,sizeof(wg_key_t*)*26);
                }
                l_key = l_key_head->lower_case_char.keys[l_str[l_iter] - 'a'];
                if (l_key == NULL)
                {
                    l_key = malloc(sizeof(wg_key_t));
                    if (NULL == l_key)
                    {
                        l_error = W_E_NO_MEMORY;
                        goto error;
                    }
                    memset(l_key,0,sizeof(*l_key));
                    l_key_head->lower_case_char.keys[l_str[l_iter] - 'a'] = l_key;
                }
                break;
            case 0:
                if (l_key_head->other_char.keys == NULL)
                {
                    l_key_head->other_char.keys = malloc(sizeof(wg_key_t*)*26);
                    if (NULL == l_key_head->other_char.keys)
                    {
                        l_error = W_E_NO_MEMORY;
                        goto error;
                    }
                    memset(l_key_head->other_char.keys,0,sizeof(wg_key_t*)*26);
                }
                l_key = l_key_head->other_char.keys[l_str[l_iter] - '!'];
                if (l_key == NULL)
                {
                    l_key = malloc(sizeof(wg_key_t));
                    if (NULL == l_key)
                    {
                        l_error = W_E_NO_MEMORY;
                        goto error;
                    }
                    memset(l_key,0,sizeof(*l_key));
                    l_key_head->other_char.keys[l_str[l_iter] - '!'] = l_key;
                }
                break;
            default:
                printf("Unmanaged character\n");
                break;
        }
        l_key_head = &l_key->u.head ;
    }
    l_key->end_of_word = true;
    l_key->udata=elem;
    return(W_NO_ERROR);
error :
    wg_key_release(key_head);
    return(l_error);
}

/**
 ** @brief Generates a key tree from a word list. 
 **/
w_error_t wg_keytree_create(keytree_list_t  *wordlist, 
                            keytree_head_t **key_head)
{
    keytree_head_t *l_key_head;
    int                    l_iter;
    w_error_t              l_error=W_NO_ERROR;

    if ((NULL == wordlist) || (NULL == key_head)){
        return(W_E_BAD_PARAMETER);
    }
    if (0 == wordlist->nb_elems)
    {
        return(W_E_BAD_PARAMETER);
    }
    if ((l_key_head = malloc(sizeof(*l_key_head))) == NULL)
    {
        printf("Failed to allocate %d bytes\n",sizeof(*l_key_head));
    }
    else 
    {
        memset(l_key_head,0,sizeof(*l_key_head));
        for (l_iter = 0; l_iter < (int)wordlist->nb_elems; l_iter ++)
        {
            if ((l_error = parse_file_create_key(&wordlist->elems[l_iter],
                                                 l_key_head)) != W_NO_ERROR)
            {
                free(l_key_head);
                l_key_head = NULL;
                printf("Failed to create a key\n");
                break;
            }
        }
        * key_head = l_key_head;
    }
    return(l_error);
}

w_error_t wg_keytree_add(keytree_head_t *head,
                         keytree_elem_t *e){
    (void)head;
    (void)e;
    return(W_NO_ERROR);
}

static wg_key_t *keytree_keyget(keytree_head_t  *key_head,
                            const char      *str){
    int                    l_iter=0;
    keytree_head_t *l_key_head = key_head;
    wg_key_t      *l_key;
    w_error_t              l_error = W_E_OPERATION_FAILED;
    const char            *l_str = str;
    (void)l_error;

    if ((NULL==key_head)||(NULL == str)){
        return(NULL);
    }

    do
    {
        l_key = NULL;
        switch(word_type(l_str[l_iter]))
        {
            case 1: /* numeric */
                if (l_key_head->number_char.keys != NULL)
                    l_key = l_key_head->number_char.keys[l_str[l_iter] - '0'];
                break;
            case 2: /* upper-case */
                if (l_key_head->upper_case_char.keys != NULL)
                    l_key = l_key_head->upper_case_char.keys[l_str[l_iter] - 'A'];
                break;
            case 3: /* lower-case */
                if (l_key_head->lower_case_char.keys != NULL)
                    l_key = l_key_head->lower_case_char.keys[l_str[l_iter] - 'a'];
                break;
            case 0:
            default:
                break;
        }
        if (NULL == l_key)
        {
            break;
        }
        else if (l_key->end_of_word == true)
        {
            /* end of word detected in tree : check the next str character. */
            if ((ispunct(l_str[l_iter+1])) || (isspace(l_str[l_iter+1])) || (iscntrl(l_str[l_iter+1])))
            {
                l_error = W_NO_ERROR;;
                return(l_key);
            }
        }
        l_key_head = &l_key->u.head;
        l_iter ++;
    }
    while(1);
    return(NULL);
}


w_error_t wg_keytree_strcmp(keytree_head_t  *key_head,
                            const char      *str)
{
    int                    l_iter=0;
    keytree_head_t *l_key_head = key_head;
    wg_key_t      *l_key;
    w_error_t              l_error = W_E_OPERATION_FAILED;
    const char            *l_str = str;

    if ((NULL == key_head)||(NULL == str)){
        return(W_E_BAD_PARAMETER);
    }

    do
    {
        l_key = NULL;
        switch(word_type(l_str[l_iter]))
        {
            case 1: /* numeric */
                if (l_key_head->number_char.keys != NULL)
                    l_key = l_key_head->number_char.keys[l_str[l_iter] - '0'];
                break;
            case 2: /* upper-case */
                if (l_key_head->upper_case_char.keys != NULL)
                    l_key = l_key_head->upper_case_char.keys[l_str[l_iter] - 'A'];
                break;
            case 3: /* lower-case */
                if (l_key_head->lower_case_char.keys != NULL)
                    l_key = l_key_head->lower_case_char.keys[l_str[l_iter] - 'a'];
                break;
            case 0:
            default:
                break;
        }
        if (NULL == l_key)
        {
            break;
        }
        else if (l_key->end_of_word == true)
        {
            /* end of word detected in tree : check the next str character. */
             if ((isspace(l_str[l_iter+1])) || (iscntrl(l_str[l_iter+1])))
            {
                l_error = W_NO_ERROR;
            }
        }
        l_key_head = &l_key->u.head;
        l_iter ++;
    }
    while(1);
    return(l_error);
}

int wg_keytree_strcmp_isalnum(keytree_head_t  *key_head,
                              const char      *str)
{
    int                    l_iter=0;
    keytree_head_t *l_key_head = key_head;
    wg_key_t      *l_key;
    w_error_t              l_error = -1;
    const char            *l_str = str;

    if ((NULL == key_head)||(NULL == str)){
        return(-1);
    }

    do
    {
        l_key = NULL;
        switch(word_type(l_str[l_iter]))
        {
            case 1: /* numeric */
                if (l_key_head->number_char.keys != NULL)
                    l_key = l_key_head->number_char.keys[l_str[l_iter] - '0'];
                break;
            case 2: /* upper-case */
                if (l_key_head->upper_case_char.keys != NULL)
                    l_key = l_key_head->upper_case_char.keys[l_str[l_iter] - 'A'];
                break;
            case 3: /* lower-case */
                if (l_key_head->lower_case_char.keys != NULL)
                    l_key = l_key_head->lower_case_char.keys[l_str[l_iter] - 'a'];
                break;
            case 0:
                if (l_key_head->other_char.keys != NULL)
                    l_key = l_key_head->other_char.keys[l_str[l_iter] - '!'];
                break;
            default:
                break;
        }
        if (NULL == l_key)
        {
            break;
        }
        else if (l_key->end_of_word == true)
        {
            /* end of word detected in tree : check the next str character. */
            if (!isalnum(l_str[l_iter+1]))
            {
                l_error = l_iter+1;
            }
        }
        l_key_head = &l_key->u.head;
        l_iter ++;
    }
    while(1);
    return(l_error);
}

w_error_t wg_keytree_foreach(keytree_head_t  *key_head,
                             wg_keytree_fn_t  fn,
                             void            *p_data){
}

w_error_t wg_keytree_destroy(keytree_head_t *key_head){
}

w_error_t wg_keytree_keyget(keytree_head_t  *key_head,
                            const char      *str,
                            keytree_elem_t **data){
    wg_key_t *l_key;
    w_error_t l_ret=W_E_OPERATION_FAILED;

    if (NULL != (l_key = keytree_keyget(key_head,str))){
        *data=l_key->udata;
        l_ret=W_NO_ERROR;
    }
    return(l_ret);
}


/**
 ** @brief Releases a key tree.
 **/
w_error_t wg_keytree_release(keytree_head_t *key_head)
{
    if (NULL == key_head){
        return(W_E_BAD_PARAMETER);
    }

    wg_key_release(key_head);
    free(key_head);
    return(W_NO_ERROR);
}


w_error_t keytree_key_init(keytree_elem_t *k,
                           const char     *str,
                           int             d){
    int len;
    w_error_t l_ret;
    if ((NULL == k) || (NULL == str)){
        l_ret=W_E_BAD_PARAMETER;
    }
    else {
        len = strlen(str);
        if (NULL == (k->w.str=malloc(len))){
            printf("Failed to allocate %d bytes\n",len);
            l_ret=W_E_NO_MEMORY;
        }
        else {
            memcpy(&k->w.str[0],&str[0],len);
            k->w.len=len;
            k->d=d;
            l_ret=0;
        }
    }
    return(l_ret);
}


w_error_t keytree_key_uninit(keytree_elem_t *k){
    w_error_t l_ret;
    if (NULL == k){
        l_ret=W_E_BAD_PARAMETER;
    }
    else {
        free(k->w.str);
    }
    return(l_ret);
}

