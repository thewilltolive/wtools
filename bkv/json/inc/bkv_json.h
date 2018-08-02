/**
** @file bkv_json.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_JSON_H_
#define  BKV_JSON_H_

#include <stdint.h>
#include "bkv.h"
#include "bkv_dico.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

typedef struct bkv_from_json_parser_s *bkv_from_json_parser_t;
typedef struct bkv_to_json_parser_s   *bkv_to_json_parser_t;

bkv_error_t bkv_from_json(bkv_dico_type_t dico_type,
                          bkv_from_json_parser_t p,
                          uint8_t *ptr,
                          int      len,
                          bkv_t   *h);

int bkv_to_json(bkv_to_json_parser_t p,
                bkv_t             h,
                uint8_t         **ptr,
                int              *len);

    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_JSON_H_  ----- */

