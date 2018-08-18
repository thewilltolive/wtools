#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "CUnit/Automated.h"

#include "bkv.h"
#include "bkv_val.h"

static int init_suite(void)
{
    return 0;
}

static int clean_suite(void)
{
    return 0;
}

static void test_bkv_init_term(void){
    bkv_init_t bkv;
    CU_ASSERT_NOT_EQUAL(BKV_OK,bkv_init(NULL));
    CU_ASSERT_EQUAL(BKV_OK,bkv_init(&bkv));
    CU_ASSERT_EQUAL(BKV_OK,bkv_term());
}

static void test_bkv_u16(void){
    bkv_init_t   l_init;
    bkv_create_t l_bkv_create=BKV_CREATE_INIT;
    bkv_t        l_handle;
    bkv_val_t    l_val=BKV_VAL_INIT;
    bkv_val_t    l_out_value;
    uint8_t     *l_ptr=NULL;
    int          l_ptrlen;
#define TEST_KEY (1)
    int16_t      l_key=TEST_KEY;
    uint16_t     l_value=32;
    uint16_t     l_keys[2] = { TEST_KEY, BKV_KEY_INVALID };
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_init(&l_init));
    l_bkv_create.create_type=BKV_CREATE_TYPE_OPEN_FILE_CREAT_AND_WRITE;
    l_bkv_create.filename="test_file.db";
    l_bkv_create.mode=0777;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_create(&l_bkv_create,&l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_open(l_handle,BKV_NO_KEY));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_u16_add(l_handle,l_key,l_value));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_close(l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_get_head(l_handle,&l_ptr,&l_ptrlen));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_init(&l_val,l_ptr));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_val,&l_keys[0],1,&l_out_value));
    CU_ASSERT_EQUAL(l_out_value.u.number.int16,l_value);
    bkv_destroy(l_handle);
    bkv_term();
}

static void test_bkv_str(void){
    bkv_init_t   l_init;
    bkv_create_t l_bkv_create=BKV_CREATE_INIT;
    bkv_t        l_handle;
   bkv_val_t    l_val=BKV_VAL_INIT;
    bkv_val_t    l_out_value;
    uint8_t     *l_ptr=NULL;
    int          l_ptrlen;
#define TEST_KEY (1)
    int16_t      l_key=TEST_KEY;
    const char  *l_value="the test content";
    uint16_t     l_keys[2] = { TEST_KEY, BKV_KEY_INVALID };

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_init(&l_init));
    l_bkv_create.create_type=BKV_CREATE_TYPE_OPEN_FILE_CREAT_AND_WRITE;
    l_bkv_create.filename="test_file.db";
    l_bkv_create.mode=0777;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_create(&l_bkv_create,&l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_open(l_handle,BKV_NO_KEY));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_str_add(l_handle,l_key,(const uint8_t*)l_value,strlen(l_value)));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_close(l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_get_head(l_handle,&l_ptr,&l_ptrlen));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_init(&l_val,l_ptr));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_val,&l_keys[0],1,&l_out_value));
    CU_ASSERT_PTR_NOT_EQUAL(NULL,l_out_value.u.string.str);
    CU_ASSERT_NOT_EQUAL(0,l_out_value.u.string.len);
    if ((l_out_value.u.string.str != NULL)&&(l_out_value.u.string.len>0)){
        uint8_t *l_str=malloc(l_out_value.u.string.len+1);
        memcpy(l_str,l_out_value.u.string.str,l_out_value.u.string.len);
        l_str[l_out_value.u.string.len]=0;
        CU_ASSERT_STRING_EQUAL(l_str,l_value);
        free(l_str);
    }
    bkv_destroy(l_handle);
    bkv_term();
}

static void test_bkv_float_multi(void){
    bkv_init_t   l_init;
    bkv_create_t l_bkv_create=BKV_CREATE_INIT;
    bkv_t        l_handle;
    bkv_val_t    l_val=BKV_VAL_INIT;
    bkv_val_t    l_out_value;
    uint8_t     *l_ptr=NULL;
    int          l_ptrlen,i;
#define FLOAT_TEST_KEY (987)
    int16_t      l_key=FLOAT_TEST_KEY;
    float        l_value=7.535434;
    uint16_t     l_keys[2] = { FLOAT_TEST_KEY, BKV_KEY_INVALID };

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_init(&l_init));
    l_bkv_create.create_type=BKV_CREATE_TYPE_OPEN_FILE_CREAT_AND_WRITE;
    l_bkv_create.filename="test_file.db";
    l_bkv_create.mode=0777;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_create(&l_bkv_create,&l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_open(l_handle,BKV_NO_KEY));
    for (i=0;i<10;i++){
        CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_float_add(l_handle,l_key+i,l_value+i));
    }
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_close(l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_get_head(l_handle,&l_ptr,&l_ptrlen));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_init(&l_val,l_ptr));
    for(i=0;i<10;i++){
        l_keys[0]=(uint16_t)(l_key+i);
        CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_val,&l_keys[0],1,&l_out_value));
        CU_ASSERT_TRUE((l_out_value.u.number.f - (l_value+i)) < 0.1);
    }
    bkv_destroy(l_handle);
    bkv_term();
}

static void test_bkv_float(void){
    bkv_init_t   l_init;
    bkv_create_t l_bkv_create=BKV_CREATE_INIT;
    bkv_t        l_handle;
    bkv_val_t    l_val=BKV_VAL_INIT;
    bkv_val_t    l_out_value;
    uint8_t     *l_ptr=NULL;
    int          l_ptrlen;
#define FLOAT_TEST_KEY (987)
    int16_t      l_key=FLOAT_TEST_KEY;
    float        l_value=7.535434;
    uint16_t     l_keys[2] = { FLOAT_TEST_KEY, BKV_KEY_INVALID };

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_init(&l_init));
    l_bkv_create.create_type=BKV_CREATE_TYPE_OPEN_FILE_CREAT_AND_WRITE;
    l_bkv_create.filename="test_file.db";
    l_bkv_create.mode=0777;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_create(&l_bkv_create,&l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_open(l_handle,BKV_NO_KEY));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_float_add(l_handle,l_key,l_value));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_close(l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_get_head(l_handle,&l_ptr,&l_ptrlen));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_init(&l_val,l_ptr));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_val,&l_keys[0],1,&l_out_value));
    CU_ASSERT_TRUE(abs(l_out_value.u.number.f - l_value) < 0.1);
    bkv_destroy(l_handle);
    bkv_term();
}

static void test_bkv_double(void){
    bkv_init_t   l_init;
    bkv_create_t l_bkv_create=BKV_CREATE_INIT;
    bkv_t        l_handle;
    bkv_val_t    l_val=BKV_VAL_INIT;
    bkv_val_t    l_out_value;
    uint8_t     *l_ptr=NULL;
    int          l_ptrlen;
#define DOUBLE_TEST_KEY (876)
    int16_t      l_key=DOUBLE_TEST_KEY;
    double       l_value=79789.535434;
    uint16_t     l_keys[2] = { DOUBLE_TEST_KEY, BKV_KEY_INVALID };

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_init(&l_init));
    l_bkv_create.create_type=BKV_CREATE_TYPE_OPEN_FILE_CREAT_AND_WRITE;
    l_bkv_create.filename="test_file.db";
    l_bkv_create.mode=0777;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_create(&l_bkv_create,&l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_open(l_handle,BKV_NO_KEY));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_double_add(l_handle,l_key,l_value));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_close(l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_get_head(l_handle,&l_ptr,&l_ptrlen));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_init(&l_val,l_ptr));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_val,&l_keys[0],1,&l_out_value));
    CU_ASSERT_TRUE(abs(l_out_value.u.number.d - l_value) < 0.1);
    bkv_destroy(l_handle);
    bkv_term();
}

static void test_bkv_array(void){
    bkv_init_t   l_init;
    bkv_create_t l_bkv_create=BKV_CREATE_INIT;
    bkv_t        l_handle;
    uint8_t     *l_ptr=NULL;
    int          l_ptrlen,i;
#define FLOAT_TEST_KEY (987)
    int16_t      l_key=FLOAT_TEST_KEY;
    float        l_value=7.535434;
    uint16_t     l_keys[2] = { FLOAT_TEST_KEY, BKV_KEY_INVALID };
    bkv_val_t    l_val=BKV_VAL_INIT;
    bkv_val_t    l_out_value[2]={BKV_VAL_INIT,BKV_VAL_INIT},l_array_value=BKV_VAL_INIT;

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_init(&l_init));
    l_bkv_create.create_type=BKV_CREATE_TYPE_OPEN_FILE_CREAT_AND_WRITE;
    l_bkv_create.filename="test_file.db";
    l_bkv_create.mode=0777;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_create(&l_bkv_create,&l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_open(l_handle,BKV_NO_KEY));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_array_open(l_handle,l_key));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_array_float_add(l_handle,l_value));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_array_float_add(l_handle,l_value));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_array_close(l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_close(l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_get_head(l_handle,&l_ptr,&l_ptrlen));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_init(&l_val,l_ptr));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_val,&l_keys[0],1,&l_array_value));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_array_value,NULL,sizeof(l_out_value)/sizeof(l_out_value[0]),&l_out_value[0]));
    CU_ASSERT_EQUAL_FATAL(l_array_value.type,BKV_VAL_TYPE_ARRAY);
    CU_ASSERT_EQUAL_FATAL(l_array_value.u.array.len,2);
    for (i=0;i<l_array_value.u.array.len;i++){
        CU_ASSERT_EQUAL_FATAL(l_out_value[i].type,BKV_VAL_TYPE_FLOAT);
        CU_ASSERT_TRUE(abs(l_out_value[i].u.number.f - l_value) < 0.1);
    }
    bkv_destroy(l_handle);
    bkv_term();
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
    pSuite = CU_add_suite("Suite dml_poi_uc00", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pSuite, "test_bkv_init_term", test_bkv_init_term)) {
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "test_bkv_u16", test_bkv_u16)) {
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "test_bkv_str", test_bkv_str)) {
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "test_bkv_float", test_bkv_float)) {
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "test_bkv_double", test_bkv_double)) {
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "test_bkv_float_multi", test_bkv_float_multi)) {
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "test_bkv_array", test_bkv_array)) {
        return CU_get_error();
    }





    /* Run all tests using the CUnit Basic interface */
    CU_set_output_filename("cunit_bkv_uc01");
    CU_automated_run_tests();
    num_of_failures = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return num_of_failures;
}
