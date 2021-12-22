
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "w_rbt.h"
#include "time.h"

#define TEST_LOG_ERROR(a, ...) do { printf("%s:%d:> ",__FUNCTION__,__LINE__); printf(a,#__VA_ARGS__); printf("\n"); } while(0)
#define TEST_LOG_INFO(a, ...) do { printf("%s:%d:> ",__FUNCTION__,__LINE__); printf(a,#__VA_ARGS__); printf("\n"); } while(0)
/*  this file has functions to test a red-black tree of integers */

static void IntDest(void* a) {
  (void) a;
}



static int rtd_zlog_key_comp(const void* a,const void* b) {
    char * key_1 = (char *)a;
    char * key_2 = (char*)b;
    if((NULL == key_1)||(NULL == key_2)){
        return (NULL != key_1) - (NULL != key_2);
    }else{
        return strcmp(key_1,key_2);
    }
    return(0);
}

static void IntPrint(const void* a) {
    printf("%s",(char*)a);
}

static void InfoPrint(void* a) {
    printf("%i",*(int*)a);
}

static void InfoDest(void *a){
    (void)a;
}

static int rbt_test_insert(int nb){
    rb_red_blk_tree* tree;
    int i = 0;
    int list_i[nb];
    char key[nb][64];
    char key_get[nb][64];
    rb_red_blk_node * l_node = NULL;
    rb_red_blk_node * l_node_get = NULL;
    tree=RBTreeCreate(rtd_zlog_key_comp,IntDest,InfoDest,IntPrint,InfoPrint);
    if(NULL == tree){
        TEST_LOG_ERROR("Failed to create a red black tree");
        return -1;
    }
    for(i=0;i<nb;i++){
        snprintf((char*)&(key[i])[0],sizeof(key[i]),"key%d",i);
        snprintf((char*)&(key_get[i])[0],sizeof(key_get[i]),"key%d",i);
        list_i[i] = i;
        if(NULL == (l_node = RBTreeInsert(tree,(void*)key[i],(void *)&list_i[i]))){
            TEST_LOG_ERROR("Failed to create a red black node");
            return -1;
        }
        if(NULL == (l_node_get = RBExactQuery(tree,(void*)key_get[i]))){
            TEST_LOG_ERROR("Failed to get the red black node");
            return -1;
        }
        IntPrint(l_node_get->key);
        printf(" ");
        InfoPrint(l_node_get->info);
        printf("\n");
    }

    RBTreeDestroy(tree);
    return 0;
}



static void rbt_validate(void){

    if(0 != rbt_test_insert(10)){
        TEST_LOG_ERROR("Invalid rbt insert 10 elts");
    }else{
        TEST_LOG_INFO("rbt insert 10 elts OK");
    }

    if(0 != rbt_test_insert(100)){
        TEST_LOG_ERROR("Invalid rbt insert 100 elts");
    }else{
        TEST_LOG_INFO("rbt insert 100 elts OK");
    }

    if(0 != rbt_test_insert(1000)){
        TEST_LOG_ERROR("Invalid rbt insert 1000 elts");
    }else{
        TEST_LOG_INFO("rbt insert 1000 elts OK");
    }

    if(0 != rbt_test_insert(10000)){
        TEST_LOG_ERROR("Invalid rbt insert 10000 elts");
    }else{
        TEST_LOG_INFO("rbt insert 10000 elts OK");
    }
}


int main(){
    rbt_validate();

      return 0;
}
