#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bkv_mem.h"
#include "bkv_error.h"

int bkv_mem_truncate(uint8_t *p_ptr, int new_size, uint8_t **pp_ptr){
    uint8_t *l_ptr;
    int      l_ret = BKV_OK;
    if (NULL == (l_ptr = realloc(p_ptr,new_size))){
        l_ret=BKV_HDL_MEM;
    }
    else {
        * pp_ptr = l_ptr;
    }
    return(l_ret);
}

