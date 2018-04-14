
#include <stdio.h>
#include <stdarg.h>
#include "bkv_print.h"

int bkv_print(const char *p_function, int line, char *p_fmt, ...){
    int     l_ret;
    va_list l_va;
    printf("%s:%d",p_function,line);
    va_start(l_va,p_fmt);
    l_ret=vprintf(p_fmt,l_va);
    va_end(l_va);
    return(l_ret);
}


