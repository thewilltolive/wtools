#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bkv_json_tools.h"
#include "bkv_error.h"
#include <yajl/yajl_tree.h>

static int json_tools_node_compare(yajl_val a, 
                                   yajl_val b){
    size_t i;
    int    r;
    if (a->type != b->type){
        return(-1);
    }
    else {
        switch(a->type){
        case yajl_t_string: 
            printf(" str %s\n",a->u.string);
            r=strcmp(a->u.string,b->u.string);
            if (r) {
                printf("strcmp(%s,%s)!=0\n",a->u.string,b->u.string);
            }
            break;
        case yajl_t_number:
            if (!abs(a->u.number.d - b->u.number.d)<0.001){
                printf(" flag %d \n",a->u.number.flags);
                printf(" flag %d \n",b->u.number.flags);
                printf("decimal %f != %f\n",a->u.number.d,b->u.number.d);
                return(-1);
            }
            break;
        case yajl_t_object:
            for (i=0;i<a->u.object.len;i++){
                if (strcmp(a->u.object.keys[i],b->u.object.keys[i])!=0){
                    return(-1);
                }
                else if (0 != json_tools_node_compare(a->u.object.values[i],
                                                       b->u.object.values[i])){
                    return(-1);
                }
            }
            break;
        case yajl_t_array:
            for (i=0;i<a->u.object.len;i++){
                if (0 != json_tools_node_compare(a->u.array.values[i],
                                                  b->u.array.values[i])){
                    return(-1);
                }
            }
            break;
        default:
            break;
        }
    }
    return(0);
}

int bkv_json_tools_compare(const uint8_t *a, 
                           int            a_len,
                           const uint8_t *b,
                           int            b_len){
    int      l_ret=BKV_INV_ARG;
#if 0
    uint8_t *l_a;
#endif
    uint8_t *l_b;
    yajl_val l_node_a = NULL;
    yajl_val l_node_b = NULL;
    (void)a_len;

    if (b[b_len-1] != 0){
        l_b=malloc(b_len+1);
        memcpy(l_b,b,b_len);
        l_b[b_len]=0;
    }
    else {
        l_b=(uint8_t*)b;
    }

    if ((l_node_a = yajl_tree_parse((const char *)a, NULL, 0)) == NULL) {
        printf("Failed to parse first string\n");
        goto exit;
    }

    if ((l_node_b = yajl_tree_parse((const char *)l_b, NULL, 0)) == NULL) {
        printf("Failed to parse second string\n");
        goto exit;
    }
    l_ret=json_tools_node_compare(l_node_a,l_node_b);


exit:
    if (b != l_b){
        free(l_b);
        l_b=NULL;
    }
    if (NULL != l_node_a){
        yajl_tree_free(l_node_a);
    }
    if (NULL != l_node_b){
        yajl_tree_free(l_node_b);
    }
    return(l_ret);
}
