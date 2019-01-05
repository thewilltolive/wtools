/**
** @file bkv_print.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_PRINT_P_H_
#define  BKV_PRINT_P_H_


#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

#ifdef DEBUG
#define BKV_FCT_INIT(fct)   const char *__function=#fct;\
                            int         line=__LINE__;\
                            (void)__function;(void)line;
#define BKV_FCT_NAME __function
#define BKV_HDL(h) bkv_print_state(__functions,line,h);
#else
#define BKV_FCT_INIT(fct)
#define BKV_FCT_NAME 
#define BKV_HDL(h) 
#endif

typedef enum {
    BKV_ERROR_ST_DEBUG,
    BKV_ERROR_ST_INFO,
    BKV_ERROR_ST_WARN,
    BKV_ERROR_ST_ERROR,
    BKV_ERROR_ST_CRIT,
    BKV_ERROR_ST_END_VALUE
} bkv_error_st_t;

int bkv_print(bkv_error_st_t error_st,
              const char *function, 
              int line, 
              char *fmt, ...);

    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_PRINT_P_H_  ----- */

