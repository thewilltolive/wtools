/**
** @file bkv_header.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_HEADER_H_
#define  BKV_HEADER_H_


#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

typedef enum {
    HDR_TYPE_MAP_OPEN=1,
    HDR_TYPE_MAP_CLOSE,
    HDR_TYPE_ARRAY_OPEN,
    HDR_TYPE_ARRAY_CLOSE,
    HDR_TYPE_INT16,
    HDR_TYPE_INT32,
    HDR_TYPE_INT64,
    HDR_TYPE_STRING,
    HDR_TYPE_FLOAT,
    HDR_TYPE_DOUBLE,
    HDR_TYPE_BOOLEAN,
    HDR_TYPE_END_VALUE

} header_type_t;

#define HDR_SIZE               (2)
#define MAP_OPEN_VALUE_SIZE    (2)
#define MAP_CLOSE_VALUE_SIZE   (0)
#define STRING_VALUE_SIZE      (2)  /*!< length of the string. */
#define ARRAY_OPEN_VALUE_SIZE  (4)  /*!<  2 bytes: array bytes length. 2 bytes: nb elems. */
#define ARRAY_CLOSE_VALUE_SIZE (3)



    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_HEADER_H_  ----- */

