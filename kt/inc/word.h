/*
** Copyright (C) 2013 Renault-Trucks Defense.
*/

/**
** @file word.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  WORD_H_
#define  WORD_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

typedef struct {
    uint8_t *str;
    int      len;
} word_t;
#define WORD_INIT {NULL,0}


/**
 ** @brief Defines a word list.
 */
typedef struct
{
    uint32_t nb_words;
    word_t  *words; 

} wordlist_t;

    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef WORD_H_  ----- */

