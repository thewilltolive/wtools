#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "CUnit/Automated.h"

#include "bkv.h"
#include "bkv_val.h"

/*!
 * @brief Tests files.
 */

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

static uint16_t s_u16_key=73;
static uint16_t s_u16_value=739;
static bkv_parse_retval_t s_uint16_callback(void *p_data, uint8_t *p_ptr, int deep_offset, bkv_key_t key, uint16_t value){
    bkv_parse_retval_t l_ret=BKV_PARSE_ACTION_NONE;
    (void)p_data;
    (void)p_ptr;
    (void)deep_offset;
    CU_ASSERT_EQUAL(key,s_u16_key);
    CU_ASSERT_EQUAL(value,s_u16_value);
    return(l_ret);
}

static void test_bkv_file_usage(void){
    bkv_init_t           l_init;
    bkv_create_t         l_bkv_create=BKV_CREATE_INIT;
    bkv_t                l_handle;
    bkv_val_t            l_val=BKV_VAL_INIT;
    uint8_t             *l_ptr=NULL;
    int                  l_ptrlen;
    const char          *l_filename="test_file.db";
    bkv_val_callbacks_t  l_callbacks = {
        NULL,NULL,NULL,NULL,
        s_uint16_callback,
        NULL,NULL,
        NULL,NULL,NULL,NULL

    };

    unlink(l_filename);

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_init(&l_init));
    l_bkv_create.create_type=BKV_CREATE_TYPE_OPEN_FILE_CREAT_AND_WRITE;
    l_bkv_create.filename="test_file.db";
    l_bkv_create.mode=0777;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_create(&l_bkv_create,&l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_open(l_handle,BKV_NO_KEY));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_u16_add(l_handle,s_u16_key,s_u16_value));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_close(l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_finalize(l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_get_head(l_handle,&l_ptr,&l_ptrlen));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_init(&l_val,l_ptr));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_foreach(&l_val,&l_callbacks,NULL));
    bkv_destroy(l_handle);
    bkv_term();
}

static void test_bkv_file_insert_bkv(void){
    bkv_init_t           l_init;
    bkv_create_t         l_bkv_create=BKV_CREATE_INIT;
    bkv_t                l_handle;
    bkv_val_t            l_val=BKV_VAL_INIT;
    bkv_val_t            l_out_value;
    uint8_t             *l_ptr=NULL;
    int                  l_ptrlen,l_i;
    const char          *l_filename="test_file.db";
    uint16_t             l_lookup_key=BKV_NO_KEY;
    uint16_t             l_lookup_array[2];
#define NB_KEYS (5)
    uint16_t             l_u16_key_array[NB_KEYS];
    uint16_t             l_u16_value_array[NB_KEYS];

    unlink(l_filename);

    for(l_i=0;l_i<NB_KEYS;l_i++){
        l_u16_key_array[l_i]=7+l_i;
        if (l_i==3){
            l_lookup_key=l_u16_key_array[l_i];
        }
        l_u16_value_array[l_i]=87+l_i*7;
    }

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_init(&l_init));
    l_bkv_create.create_type=BKV_CREATE_TYPE_OPEN_FILE_CREAT_AND_WRITE;
    l_bkv_create.filename="test_file.db";
    l_bkv_create.mode=0777;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_create(&l_bkv_create,&l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_open(l_handle,BKV_NO_KEY));
    for(l_i=0;l_i<NB_KEYS;l_i++){
        CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_u16_add(l_handle,l_u16_key_array[l_i],l_u16_value_array[l_i]));
    }
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_close(l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_finalize(l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_get_head(l_handle,&l_ptr,&l_ptrlen));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_init(&l_val,l_ptr));
    l_lookup_array[0]=l_lookup_key;
    l_lookup_array[1]=BKV_KEY_INVALID;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_val,&l_lookup_array[0],1,&l_out_value));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_write_offset_set(l_handle,&l_out_value));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_u16_add(l_handle,333,3333));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_finalize(l_handle));

    /* look for the key after the insertion. */
    l_lookup_array[0]=l_lookup_key+1;
    l_lookup_array[1]=BKV_KEY_INVALID;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_val,&l_lookup_array[0],1,&l_out_value));
    /* look for the value of this key. */
    for(l_i=0;l_i<NB_KEYS;l_i++){
        if (l_u16_key_array[l_i] == (l_lookup_key+1)){
            break;
        }
    }
    CU_ASSERT_TRUE(l_i<NB_KEYS);
    CU_ASSERT_EQUAL_FATAL(l_out_value.type,BKV_VAL_TYPE_INT16);
    CU_ASSERT_EQUAL_FATAL(l_u16_value_array[l_i],l_out_value.u.number.int16);
    /* check the data list. */
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
    if (NULL == CU_add_test(pSuite, "test_bkv_file_usage", test_bkv_file_usage)) {
        return CU_get_error();
    }
    if (NULL == CU_add_test(pSuite, "test_bkv_file_insert_bkv", test_bkv_file_insert_bkv)) {
        return CU_get_error();
    }



    /* Run all tests using the CUnit Basic interface */
    CU_set_output_filename("cunit_bkv_uc01");
    CU_automated_run_tests();
    num_of_failures = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return num_of_failures;
}
