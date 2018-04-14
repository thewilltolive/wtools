/**
** @file bkv_print.h
**
** @brief
**  TBD
**
** @version
**  1.0
*/

#ifndef  BKV_PRINT_H_
#define  BKV_PRINT_H_


#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

#ifdef DEBUG
#define BKV_FCT_INIT(fct)   const char *s=#fct;\
                            int         line=__LINE__;\
                            (void)s;(void)line;
#define BKV_FCT_NAME s
#define BKV_HDL(h) bkv_print_state(s,line,h);
#else
#define BKV_FCT_INIT(fct)
#define BKV_FCT_NAME 
#define BKV_HDL(h) 
#endif

int bkv_print(const char *function, int line, char *fmt, ...);

    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef BKV_PRINT_H_  ----- */

