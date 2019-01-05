#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "w_errors.h"
#include "keytree.h"

/*!
** @brief Returns word type.
** @note 1 for a number.
**       2 for an upper-case character.
**       3 for a lower-case character.
**       0 for other values such as character with accent.
*/
static uint8_t word_type(const char c){
    if isdigit(c)
        return(1);
    if (isalpha(c) != 0){
        if isupper(c)
            return(2);
        if islower(c)
            return(3);
    }
    return(0);
}

/*!
** @brief Returns word type.
** @note 1 for a number.
**       3 for e,é,è
**       2 for an upper-case character.
**       3 for a lower-case character.
**       0 for other values such as character with accent.
*/
static uint8_t word_weak_type(kt_weak_type_t      weak_type,
                              const unsigned char c,
                              unsigned char      *ret_c){
    if (c == '\n'){
        return(-1);
    }
    if isdigit(c){
         *ret_c=c;
        return(1);
    }
    if ((weak_type & KT_WEAK_TYPE_CASE_SENSITIVE) && (c >= 'A') && (c <= 'Z')){
        *ret_c='a'+(c-'A');

    }
    else {
        *ret_c=c;
    }
    switch(*ret_c){
    case 0xe8:
    case 0xe9:
    case 0xeA:
    case 0xeB:
    case 0xc3:
        *ret_c='e';
        break;
    case ' ':
    case '?':
        return -1;
        break;
    default:
        break;
    }
    if (isalpha(*ret_c) != 0){
        if isupper(*ret_c)
            return(2);
        if islower(*ret_c)
            return(3);
    }
    return(0);
}

static w_error_t keytree_foreach(keytree_head_t *k,
                                 wg_keytree_fn_t fn,
                                 uint8_t        *p_str,
                                 int             strlen,
                                 void           *p_data){
    uint32_t        l_i;
    int             l_found,l_nb_keys;
    wg_key_t       *l_key;


    l_found=0;
    l_nb_keys=k->lower_case_char.nb_keys;
    for(l_i=0;l_i<26&&l_found<l_nb_keys;l_i++){
        l_key=k->lower_case_char.keys[l_i];
        if (NULL == l_key){
            continue;
        }
        l_found++;
        p_str[strlen]='a'+l_i;
        fn(l_key,&p_str[0],strlen+1,p_data);
        keytree_foreach(&l_key->u.head,
                        fn,&p_str[0],strlen+1,p_data);
    }
    l_found=0;
    l_nb_keys=k->upper_case_char.nb_keys;
    for(l_i=0;l_i<26&&l_found<l_nb_keys;l_i++){
        l_key=k->upper_case_char.keys[l_i];
        if (NULL == l_key){
            continue;
        }
        l_found++;
        p_str[strlen]='A'+l_i;
        fn(l_key,&p_str[0],strlen+1,p_data);
        keytree_foreach(&l_key->u.head,
                        fn,&p_str[0],strlen+1,p_data);
    }
    l_found=0;
    l_nb_keys=k->number_char.nb_keys;
    for(l_i=0;l_i<10&&l_found<l_nb_keys;l_i++){
        l_key=k->number_char.keys[l_i];
        if (NULL == l_key){
            continue;
        }
        l_found++;
        p_str[strlen]='0'+l_i;
        fn(l_key,&p_str[0],strlen+1,p_data);
        keytree_foreach(&l_key->u.head,
                        fn,&p_str[0],strlen+1,p_data);
    }
    l_found=0;
    l_nb_keys=k->other_char.nb_keys;
    for(l_i=0;l_i<k->other_char.nb_keys;l_i++){
        l_key=k->other_char.keys[l_i];
        if (NULL == l_key){
            continue;
        }
        l_found++;
        p_str[strlen]='a'+l_i;
        fn(l_key,&p_str[0],strlen+1,p_data);
        keytree_foreach(&l_key->u.head,
                        fn,&p_str[0],strlen+1,p_data);
    }
    return(W_NO_ERROR);
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

static w_error_t kt_key_add(wg_key_list_t *charlist,
                            unsigned char  offset,
                            int            nb_characters,
                            wg_key_t     **p_key,
                            bool          *new_elem){
    w_error_t l_error=W_NO_ERROR;

    if (charlist->keys == NULL){
        charlist->keys = malloc(sizeof(wg_key_t*)*nb_characters);
        if (NULL == charlist->keys){
            l_error = W_E_NO_MEMORY;
            goto error;
        }
        memset(charlist->keys,0,sizeof(wg_key_t*)*nb_characters);
    }
    *p_key = charlist->keys[offset];
    if (*p_key == NULL){
        *p_key = malloc(sizeof(wg_key_t));
        if (NULL == *p_key){
            l_error = W_E_NO_MEMORY;
            goto error;
        }
        memset(*p_key,0,sizeof(**p_key));
        charlist->keys[offset] = *p_key;
        charlist->nb_keys++;
        *new_elem=true;
    }
error:
    return(l_error);
}

/**
 ** @brief Generates a key tree from a word list. 
 **/
static w_error_t parse_file_create_key(keytree_elem_t  *elem, 
                                       keytree_head_t  *key_head,
                                       void           **udata){
    w_error_t             l_error;
    word_t                *word=&elem->w;
    const uint8_t         *l_str=word->str;
    int                    l_iter;
    keytree_head_t        *l_key_head = key_head;
    wg_key_t              *l_key;
    bool                   l_new_elem;
    unsigned char          l_char;
    int                    l_offset=0;

    if (0 >= word->len){
        return(W_E_BAD_PARAMETER);
    }

    for (l_iter = 0; l_iter < (int) word->len; l_iter ++){
        l_new_elem=false;
        //printf(" add %c\n",l_str[l_iter]);
        switch(word_weak_type(key_head->type,l_str[l_iter],&l_char)){
        case 1: /* alpha num */
            if (l_key_head->number_char.keys == NULL){
                l_key_head->number_char.keys = malloc(sizeof(wg_key_t*)*10);
                if (NULL == l_key_head->number_char.keys){
                    l_error = W_E_NO_MEMORY;
                    goto error;
                }
                memset(l_key_head->number_char.keys,0,sizeof(wg_key_t*)*10);
            }
            l_key = l_key_head->number_char.keys[l_str[l_iter] - '0'];
            if (l_key == NULL){
                l_key = malloc(sizeof(wg_key_t));
                if (NULL == l_key){
                    l_error = W_E_NO_MEMORY;
                    goto error;
                }
                memset(l_key,0,sizeof(*l_key));
                l_key_head->number_char.keys[l_str[l_iter] - '0'] = l_key;
                l_key_head->number_char.nb_keys++;
                l_new_elem=true;
            }
            break;
        case 2: /* upper case */
            kt_key_add(&l_key_head->upper_case_char,l_str[l_iter] - 'A',26,&l_key,&l_new_elem);
            break;
        case 3: /* lower case */
            kt_key_add(&l_key_head->lower_case_char,l_char - 'a',26,&l_key,&l_new_elem);
            break;
        case 0:
            if (l_key_head->other_char.keys == NULL){
                l_key_head->other_char.keys = malloc(sizeof(wg_key_t*)*26);
                if (NULL == l_key_head->other_char.keys){
                    l_error = W_E_NO_MEMORY;
                    goto error;
                }
                memset(l_key_head->other_char.keys,0,sizeof(wg_key_t*)*26);
            }
            //printf(" %x offset %d\n",l_str[l_iter],l_str[l_iter] - '!');
            l_offset=l_str[l_iter] - '!';
            if ((0 > l_offset) || (26 < l_offset)){
                printf(" l_offset %d\n",l_offset);
                break;
            }
            l_key = l_key_head->other_char.keys[l_offset];
            if (l_key == NULL){
                l_key = malloc(sizeof(wg_key_t));
                if (NULL == l_key){
                    l_error = W_E_NO_MEMORY;
                    goto error;
                }
                memset(l_key,0,sizeof(*l_key));
                l_key_head->other_char.keys[l_str[l_iter] - '!'] = l_key;
                l_key_head->other_char.nb_keys++;
                l_new_elem=true;
            }
            break;
        default:
            printf("Unmanaged character\n");
            break;
        }
        l_key_head = &l_key->u.head ;
    }
    l_key->end_of_word = true;
    if (l_new_elem == false){
        if (NULL != udata){
            *udata=l_key->udata;
        }
        l_error=W_E_EXISTS;
    }
    else {
        l_key->udata=elem->udata;
        l_error=W_NO_ERROR;
    }
    if (NULL != key_head->cb){
        key_head->cb(l_key,word->str,word->len);
    }
    return(l_error);
error :
    wg_key_release(key_head);
    return(l_error);
}

/**
 ** @brief Generates a key tree from a word list. 
 **/
w_error_t wg_keytree_create(kt_create_params_t  *create_params,
                            keytree_list_t      *wordlist, 
                            keytree_head_t     **key_head){
    keytree_head_t *l_key_head;
    int                    l_iter;
    w_error_t              l_error=W_NO_ERROR;

    if ((NULL == wordlist) || (NULL == key_head)){
        return(W_E_BAD_PARAMETER);
    }
#if 0
    if (0 == wordlist->nb_elems)
    {
        return(W_E_BAD_PARAMETER);
    }
#endif
    if ((l_key_head = malloc(sizeof(*l_key_head))) == NULL){
        printf("Failed to allocate %zu bytes\n",sizeof(*l_key_head));
    }
    else 
    {
        memset(l_key_head,0,sizeof(*l_key_head));
        l_key_head->type=create_params->create_flags;
        l_key_head->cb=create_params->cb;
        for (l_iter = 0; l_iter < (int)wordlist->nb_elems; l_iter ++)
        {
            if ((l_error = parse_file_create_key(&wordlist->elems[l_iter],
                                                 l_key_head,
                                                 NULL)) != W_NO_ERROR)
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

w_error_t wg_keytree_add(keytree_head_t  *head,
                         keytree_elem_t  *e,
                         void           **udata){
    if ((NULL == head) || (NULL == e)){
        return(W_E_BAD_PARAMETER);
    }
    return(parse_file_create_key(e,head,udata));
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

w_error_t wg_keytree_weakcmp(keytree_head_t  *key_head,
                             kt_weak_type_t   weak_type,
                            const char      *str)
{
    int                    l_iter=0,l_offset;
    keytree_head_t        *l_key_head = key_head;
    wg_key_t              *l_key;
    w_error_t              l_error = W_E_OPERATION_FAILED;
    uint8_t                l_word_type;
    const char            *l_str = str;
    unsigned char          l_character;

    (void)weak_type;

    if ((NULL == key_head)||(NULL == str)){
        return(W_E_BAD_PARAMETER);
    }

    do
    {
        l_key = NULL;
        l_word_type=word_weak_type(key_head->type,l_str[l_iter],&l_character);
        if (1 == l_word_type){
                if (l_key_head->number_char.keys != NULL)
                    l_key = l_key_head->number_char.keys[l_str[l_iter] - '0'];
        }
        else { 
            l_offset=l_character - 'a';
            if (l_key_head->lower_case_char.keys != NULL)
                l_key = l_key_head->lower_case_char.keys[l_offset];
        }
        if (NULL == l_key){
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

    uint8_t str[128];
    return(keytree_foreach(key_head,fn,&str[0],0,p_data));
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
                           void           *udata){
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
            k->udata=udata;
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

