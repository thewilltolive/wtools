#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "CUnit/Automated.h"

#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>
#include "bkv_json_tools.h"
#include "bkv_json_yajl.h"

#define TEST_NAME() printf("Start Test:%s:\n",__FUNCTION__)

static int init_suite(void){
    return 0;
}

static int clean_suite(void){
    return 0;
}

static bool test_is_valid_json(const uint8_t *str,
                               int            strlen){
    bool                     l_ret=false;
    yajl_handle              l_yajl_handle;
    yajl_status              l_status;
    yajl_callbacks           callbacks = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    };


    if ((l_yajl_handle = yajl_alloc(&callbacks, NULL, NULL)) == NULL) {
    }
    if ((l_status = yajl_parse(l_yajl_handle, str, strlen)) != yajl_status_ok) {
    }
    else {
        l_ret=true;
    }
    yajl_free(l_yajl_handle);
    return(l_ret);
}

static void test_bkv_json_to_bkv_direct(void){
    bkv_from_json_parser_t  from=bkv_from_json_yajl_parser_get();
    bkv_to_json_parser_t    to=bkv_to_json_yajl_parser_get();
    const char*             str1="{ \"init\" : 0 }";
    const char*             str2="{ \"v1\" : \"string\", \"v2\" : 3}";
    const char*             str3="{ \"v1\" : \"string\", \"v2\" : { \"v3\" : 3, \"v4\" : \"str\" } }";
    bkv_t                   h;
    uint8_t                *out_str;
    int                     out_str_len;
    uint8_t                *l_str;
    int                     l_str_len;

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_from_json(BKV_DICO_TYPE_DIRECT,
                                               from,(uint8_t*)str1,
                                               strlen(str1),&h));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_to_json(to,h,&out_str,&out_str_len));
    CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)str1,strlen(str1),(const uint8_t*)out_str,out_str_len));

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_from_json(BKV_DICO_TYPE_DIRECT,
                                               from,(uint8_t*)str2,strlen(str2),&h));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_to_json(to,h,&out_str,&out_str_len));
    CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)str2,strlen(str2),(const uint8_t *)out_str,out_str_len));

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_from_json(BKV_DICO_TYPE_DIRECT,
                                               from,(uint8_t*)str3,strlen(str3),&h));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_to_json(to,h,&out_str,&out_str_len));
    CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)str3,strlen(str3),(const uint8_t*)out_str,out_str_len));
    bkv_get_head(h,&l_str,&l_str_len);
    printf(" LEN %d BKV LEN %d\n",strlen(str3),l_str_len);

    bkv_from_json_yajl_parser_rel(from);
    bkv_to_json_yajl_parser_rel(to);
}

static void test_bkv_json_to_bkv_cached(void){
    bkv_from_json_parser_t  from=bkv_from_json_yajl_parser_get();
    bkv_to_json_parser_t    to=bkv_to_json_yajl_parser_get();
    const char*             str1="{ \"init\" : 0 }";
    const char*             str2="{ \"v1\" : \"string\", \"v2\" : 3}";
    const char*             str3="{ \"v1\" : \"string\", \"v2\" : { \"v3\" : 3, \"v4\" : \"str\" } }";
    bkv_t                   h;
    uint8_t                *out_str;
    int                     out_str_len;
    uint8_t                *l_str;
    int                     l_str_len;

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_from_json(BKV_DICO_TYPE_CACHED,
                                               from,(uint8_t*)str1,
                                               strlen(str1),&h));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_to_json(to,h,&out_str,&out_str_len));
    CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)str1,strlen(str1),(const uint8_t*)out_str,out_str_len));

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_from_json(BKV_DICO_TYPE_CACHED,
                                               from,(uint8_t*)str2,strlen(str2),&h));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_to_json(to,h,&out_str,&out_str_len));
    CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)str2,strlen(str2),(const uint8_t *)out_str,out_str_len));

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_from_json(BKV_DICO_TYPE_CACHED,
                                               from,(uint8_t*)str3,strlen(str3),&h));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_to_json(to,h,&out_str,&out_str_len));
    CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)str3,strlen(str3),(const uint8_t*)out_str,out_str_len));
    bkv_get_head(h,&l_str,&l_str_len);
    printf(" LEN %d BKV LEN %d\n",strlen(str3),l_str_len);

    bkv_from_json_yajl_parser_rel(from);
    bkv_to_json_yajl_parser_rel(to);
}

static int test_bkv_get_array_elem_float(yajl_gen g,int level){
    int i;
    for (i=0;i<level+1;i++){
        yajl_gen_double(g,3.434+i+1);
    }
    return(0);
}

static int test_bkv_create_json(int        harness, 
                                int        array_deepness,
                                int      (*gen_array_elem)(yajl_gen g,int level),
                                uint8_t ** p_str,
                                int       *p_len){
    yajl_gen    l_gen;
    yajl_handle l_handle;
    uint8_t     *l_ptr;
    int          i,j,l_ret=-1;
    size_t       l_len;
    char         l_buffer[16];

    if (NULL == (l_gen = yajl_gen_alloc(NULL))) {
        printf("Failed to allocate gen handle\n");
    }
    else if(0 == (l_ret = yajl_gen_config(l_gen, yajl_gen_validate_utf8, 1))) {
        printf("Failed to configure yajl generator\n");
    }
    else if (NULL == ( l_handle = yajl_alloc(NULL, NULL, (void *) l_gen))) {
        printf("Failed to allocate yajl handle\n");
    }
    else if(!(l_ret = yajl_config(l_handle, yajl_allow_comments, 1))) {
        printf("Failed to configure handle\n");
    }

    if (yajl_gen_status_ok != yajl_gen_map_open(l_gen)){
        printf("Failed to open map");
        return(-1);
    }


    for (i = 0; i < harness; i ++){
        if (yajl_gen_status_ok != yajl_gen_string(l_gen,(unsigned char*)"test",strlen("test"))){
            printf("Failed to open map");
            return(-1);
        }

        if (yajl_gen_status_ok != yajl_gen_map_open(l_gen)){
            printf("Failed to open map");
            return(-1);
        }
        l_len = snprintf(&l_buffer[0],sizeof(l_buffer),"test%d",i);
        if (yajl_gen_status_ok != yajl_gen_string(l_gen,(unsigned char*)&l_buffer[0],l_len)){
            printf("Failed to open map");
            return(-1);
        }

        l_len = snprintf(&l_buffer[0],sizeof(l_buffer),"value%d",i);
        if (yajl_gen_status_ok != yajl_gen_string(l_gen,(unsigned char*)&l_buffer[0],l_len)){
            printf("Failed to open map");
            return(-1);
        }
        if (array_deepness > 0){

            if (yajl_gen_status_ok != yajl_gen_string(l_gen,(unsigned char*)"array",strlen("array"))){
                printf("Failed to open map");
                return(-1);
            }

            if (yajl_gen_status_ok != yajl_gen_array_open(l_gen)){
                printf("Failed to open array\n");
            }
            for (j=0;j<array_deepness;j++){
                gen_array_elem(l_gen,j);
            }
            if (yajl_gen_status_ok != yajl_gen_array_close(l_gen)){
                printf("Failed to open array\n");
            }
        }

        if (yajl_gen_status_ok != yajl_gen_map_close(l_gen)){
            printf("Failed to open map");
            return(-1);
        }
    }
    if (yajl_gen_status_ok != yajl_gen_map_close(l_gen)){
        printf("Failed to open map");
        return(-1);
    }
    if((l_ret = yajl_gen_get_buf(l_gen, (const unsigned char **)&l_ptr, &l_len))) {
        printf("Failed to generate buffer \n");
    }
    else{
        *p_str=malloc(l_len+1);
        memcpy(*p_str,l_ptr,l_len);
        (*p_str)[l_len]=0;
        *p_len=l_len;
        l_ret=BKV_OK;
    }
    yajl_gen_free(l_gen);
    yajl_free(l_handle);
    return(l_ret);
}

static void test_bkv_json_to_bkv_random(void){
    bkv_from_json_parser_t  from=bkv_from_json_yajl_parser_get();
    bkv_to_json_parser_t    to=bkv_to_json_yajl_parser_get();
    bkv_t                   h;
    uint8_t                *l_str,*l_out_str;
    int                     l_str_len,l_out_str_len;

    test_bkv_create_json(3,0,NULL,&l_str,&l_str_len);
    CU_ASSERT_EQUAL_FATAL(true,test_is_valid_json(l_str,l_str_len));
    printf(" IN %.*s\n",l_str_len,l_str);
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_from_json(BKV_DICO_TYPE_DIRECT,
                                               from,(uint8_t*)l_str,l_str_len,&h));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_to_json(to,h,&l_out_str,&l_out_str_len));
    printf("OUT %.*s\n",l_out_str_len,l_out_str);
    CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)l_str,l_str_len,(const uint8_t*)l_out_str,l_out_str_len));

    bkv_from_json_yajl_parser_rel(from);
    bkv_to_json_yajl_parser_rel(to);
    bkv_destroy(h);
    free(l_str);
    free(l_out_str);
}

static void test_bkv_json_to_bkv_random_cached(void){
    bkv_from_json_parser_t  from=bkv_from_json_yajl_parser_get();
    bkv_to_json_parser_t    to=bkv_to_json_yajl_parser_get();
    bkv_t                   h;
    uint8_t                *l_str,*l_out_str;
    int                     l_str_len,l_out_str_len;

    TEST_NAME();

    test_bkv_create_json(3,0,NULL,&l_str,&l_str_len);
    printf(" IN %.*s\n",l_str_len,l_str);
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_from_json(BKV_DICO_TYPE_CACHED,
                                               from,(uint8_t*)l_str,l_str_len,&h));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_to_json(to,h,&l_out_str,&l_out_str_len));
    printf("OUT %.*s\n",l_out_str_len,l_out_str);
    CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)l_str,l_str_len,(const uint8_t*)l_out_str,l_out_str_len));

    bkv_from_json_yajl_parser_rel(from);
    bkv_to_json_yajl_parser_rel(to);
    bkv_destroy(h);
    free(l_str);
    free(l_out_str);
}

static void test_bkv_json_to_bkv_float_array_random(void){
    bkv_from_json_parser_t  from=bkv_from_json_yajl_parser_get();
    bkv_to_json_parser_t    to=bkv_to_json_yajl_parser_get();
    bkv_t                   h;
    uint8_t                *l_str,*l_out_str;
    int                     l_str_len,l_out_str_len;

    test_bkv_create_json(3,5,test_bkv_get_array_elem_float,&l_str,&l_str_len);
    printf(" IN %.*s\n",l_str_len,l_str);
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_from_json(BKV_DICO_TYPE_DIRECT,
                                               from,(uint8_t*)l_str,l_str_len,&h));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_to_json(to,h,&l_out_str,&l_out_str_len));
    printf("OUT json_len %d bkv_len %d %.*s\n",l_str_len,bkv_size(h),l_out_str_len,l_out_str);
    CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)l_str,l_str_len,(const uint8_t*)l_out_str,l_out_str_len));

    bkv_from_json_yajl_parser_rel(from);
    bkv_to_json_yajl_parser_rel(to);
    bkv_destroy(h);
    free(l_str);
    free(l_out_str);
}

int main(int argc, char **argv)
{
    CU_pSuite pSuite = NULL;
    int num_of_failures = 0;
    (void) argc;
    (void) argv;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite bkv_to_json", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pSuite, "test_bkv_to_json direct", test_bkv_json_to_bkv_direct)) {
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pSuite, "test_bkv_to_json cached", test_bkv_json_to_bkv_cached)) {
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pSuite, "test_bkv_to_json_random", test_bkv_json_to_bkv_random)) {
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pSuite, "test_bkv_to_json_random cached", test_bkv_json_to_bkv_random_cached)) {
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pSuite, "test_bkv_to_json_random array", test_bkv_json_to_bkv_float_array_random)) {
        return CU_get_error();
    }



    /* Run all tests using the CUnit Basic interface */
    CU_set_output_filename("cunit_bkv_json_uc01");
    CU_automated_run_tests();
    num_of_failures = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return num_of_failures;
}
