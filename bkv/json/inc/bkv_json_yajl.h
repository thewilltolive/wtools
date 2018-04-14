/**
** @file bkv_json_yajl.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_JSON_YAJL_H_
#define  BKV_JSON_YAJL_H_

#include "bkv_json.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

bkv_from_json_parser_t bkv_from_json_yajl_parser_get(void);
bkv_to_json_parser_t   bkv_to_json_yajl_parser_get(void);
void bkv_from_json_yajl_parser_rel(bkv_from_json_parser_t p);
void bkv_to_json_yajl_parser_rel(bkv_to_json_parser_t p);

    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_JSON_YAJL_H_  ----- */

