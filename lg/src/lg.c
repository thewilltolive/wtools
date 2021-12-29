#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "lg.h"

typedef struct {
    char       *libname; /*!< Name of the library. */
    lg_level_t  level;   /*!< Current log level of the given library.*/
} lg_lib_t;
typedef struct {
    lg_type_t type;
    lg_lib_t  *libs;
    int        nb_libs;
}lg_process_ctx_t;
#define LG_PROCESS_CTX_INIT {LG_TYPE_END_VALUE,NULL,0}

static lg_process_ctx_t s_current_process_ctx=LG_PROCESS_CTX_INIT;

int lg_init(lg_init_params_t *p_params){
    if (NULL == p_params){
        return(LG_E_BAD_PARAMETER);
    }
    s_current_process_ctx.type=p_params->type;
    return(LG_NO_ERROR);
}

int lg_term(void){
    int l_i;

    s_current_process_ctx.type=LG_TYPE_END_VALUE;
    for (l_i=0;l_i<s_current_process_ctx.nb_libs;l_i++){
        if (NULL != s_current_process_ctx.libs[l_i].libname){
            free(s_current_process_ctx.libs[l_i].libname);
        }
    }
    if (NULL != s_current_process_ctx.libs){
        free(s_current_process_ctx.libs);
        s_current_process_ctx.libs=NULL;
    }
    return(0);
}

int lg_print(lg_level_t     level,
              const char   *p_file,
              int           line,
              const char   *p_function,
              char         *p_fmt, ...){
    int         l_ret;
    va_list     l_va;
    const char *l_st_str=NULL;

    switch(level){
    case LG_LEVEL_DEBUG:
        l_st_str="DEBG";
        break;
    case LG_LEVEL_INFO:
        l_st_str="INFO";
        break;
    case LG_LEVEL_WARN:
        l_st_str="WARN";
        break;
    case LG_LEVEL_ERROR:
        l_st_str="ERRO";
        break;
    case LG_LEVEL_CRIT:
        l_st_str="CRIT";
        break;
    default:
        l_st_str=NULL;
        break;
    }
    if (NULL == p_function){
        printf("%s:%s:%d:> ",l_st_str,p_file,line);
    } else {
        printf("%s:%s:%d:%10s> ",l_st_str,p_file,line,p_function);
    }
    va_start(l_va,p_fmt);
    l_ret=vprintf(p_fmt,l_va);
    printf("\n");
    va_end(l_va);
    return(l_ret);
}

int lg_lib_add(const char *libname){
    int       l_ret=-1,l_len;
    lg_lib_t *l_libs=NULL;
    char     *l_libname=NULL;
    l_len=strlen(libname)+1;
    if (NULL == (l_libs = realloc(s_current_process_ctx.libs,sizeof(*l_libs)*(s_current_process_ctx.nb_libs+1)))){
    }
    else if (NULL == (l_libname = malloc(l_len))){
        free(l_libs);
    }
    else {
        strncpy(l_libname,libname,l_len);
        l_libs[s_current_process_ctx.nb_libs].libname=l_libname;
        s_current_process_ctx.libs=l_libs;
        s_current_process_ctx.nb_libs++;
        l_ret=0;
    }
    return(l_ret);
}
