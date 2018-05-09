/**
** @file bkv_error.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_ERROR_H_
#define  BKV_ERROR_H_


#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */


typedef enum {
    BKV_OK        = 1,
    BKV_KO        = -1,
    BKV_HDL_INV   = -2, /*!< Invalid Handle, already closed, ...*/
    BKV_HDL_ERR   = -3, /*!< Handle with wrong creation option for this operation. */
    BKV_HDL_MEM   = -4, /*!< Memory allocation Issue. */
    BKV_INV_ARG   = -5, /*!< Invalid Argument. */
    BKV_INV_STATE = -6, /*!< Invalid Argument. */
    BKV_KEY_OUT_OF_RANGE
} bkv_error_t;


    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_ERROR_H_  ----- */

