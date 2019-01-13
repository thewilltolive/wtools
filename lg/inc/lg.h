/**
** @file lg.h
**
** @brief
** LoG.
**
** @version
**  1.0
*/

#ifndef  LG_H_
#define  LG_H_


#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *@addtogroup  [BASENAME]
     *@{
     */

typedef enum
{
    LG_NO_ERROR,
    LG_E_NOT_INITIALIZED,
    LG_E_OPERATION_FAILED,
    LG_E_AGAIN,
    LG_E_NO_MEMORY,
    LG_E_BAD_PARAMETER,
    LG_E_EXISTS

}lg_error_t;

typedef enum {
    LG_LEVEL_DEBUG,
    LG_LEVEL_INFO,
    LG_LEVEL_WARN,
    LG_LEVEL_ERROR,
    LG_LEVEL_CRIT,
    LG_LEVEL_END_VALUE
} lg_level_t;

typedef enum {
    LG_TYPE_SYSLOG,
    LG_TYPE_STDOUT,
    LG_TYPE_END_VALUE
}lg_type_t;

typedef struct{
    lg_type_t type;
}lg_init_params_t;
#define LG_INIT_PARAMS_INIT {LG_TYPE_END_VALUE}

#define LG_ASSERT_NOT_NULL(arg) if (arg == NULL) { \
        lg_print(LG_LEVEL_ERROR,__FUNCTION__,__LINE__,\
                 "Invalid argument "#arg);\
        return(LG_E_BAD_PARAMETER);}

/*!
 * @brief Initializes the Log Wrapper library for current process.
 */
extern int lg_init(lg_init_params_t *p_params);

/*!
 * @brief Terminates the Log Wrapper library for current process.
 */
extern int lg_term(void);

extern int lg_lib_add(const char *libname);

extern int lg_print(lg_level_t   level,
             const char *p_function, 
             int line, 
             char *p_fmt, ...);

    /**
     *@}
     * End of group
     */

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef LG_H_  ----- */

