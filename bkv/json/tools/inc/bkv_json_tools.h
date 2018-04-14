/*
*/

/**
** @file bkv_json_tools.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_JSON_TOOLS_H_
#define  BKV_JSON_TOOLS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */
int bkv_json_tools_compare(const uint8_t *a, 
                           int            a_len,
                           const uint8_t *b,
                           int            b_len);

    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_JSON_TOOLS_H_  ----- */

