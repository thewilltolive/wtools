/**
** @file bkv_mem.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_MEM_H_
#define  BKV_MEM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

int bkv_mem_truncate(uint8_t *p_ptr, int new_size, uint8_t **pp_ptr);

    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_MEM_H_  ----- */

