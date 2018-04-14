/**
** @file bkv_json_simple.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_JSON_SIMPLE_H_
#define  BKV_JSON_SIMPLE_H_

#include "bkv_json.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

bkv_json_parser_t bkv_json_simple_parser_get(void);
void bkv_json_simple_parser_rel(bkv_json_parser_t p);


    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_JSON_SIMPLE_H_  ----- */

