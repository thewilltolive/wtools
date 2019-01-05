#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "CUnit/Automated.h"

#include "bkv.h"
#include "bkv_val.h"

/*!
 * @brief Deep test array.
 */

static int init_suite(void)
{
    return 0;
}

static int clean_suite(void)
{
    return 0;
}

static void test_bkv_uc03_deep_search(void){
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
    uint16_t     l_search_1[2] = { TEST_KEY, BKV_KEY_INVALID };
    uint16_t     l_search_2_ko[3] = { TEST_KEY, TEST_KEY+1, BKV_KEY_INVALID };
    uint16_t     l_search_2_ok[3] = { TEST_KEY+1, TEST_KEY+2, BKV_KEY_INVALID };
    uint16_t     l_search_3[2] = { TEST_KEY+3, BKV_KEY_INVALID };
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_init(&l_init));
    l_bkv_create.create_type=BKV_CREATE_TYPE_WORK_IN_RAM;
    l_bkv_create.mode=0777;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_create(&l_bkv_create,&l_handle));

    /* create bkv:
       { key : l_value ,
         key+1 : {
           key+2:l_value+1
         },
         key+3:l_value+2
       }
    */

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_open(l_handle,BKV_NO_KEY));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_u16_add(l_handle,l_key++,l_value));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_open(l_handle,l_key++));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_u16_add(l_handle,l_key++,l_value+1));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_close(l_handle));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_u16_add(l_handle,l_key++,l_value+2));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_close(l_handle));

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_get_head(l_handle,&l_ptr,&l_ptrlen));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_init(&l_val,l_ptr));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_val,&l_search_1[0],1,&l_out_value));
    CU_ASSERT_EQUAL(l_out_value.u.number.int16,l_value);
    l_out_value.u.number.int16=-1;
    CU_ASSERT_EQUAL_FATAL(BKV_INV_ARG,bkv_val_get2(&l_val,&l_search_2_ko[0],1,&l_out_value));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_val,&l_search_2_ok[0],1,&l_out_value));
    CU_ASSERT_EQUAL(l_out_value.u.number.int16,l_value+1);
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_val,&l_search_3[0],1,&l_out_value));
    CU_ASSERT_EQUAL(l_out_value.u.number.int16,l_value+2);
    bkv_destroy(l_handle);
    bkv_term();
}

static void test_bkv_uc03_deep_array_search(void){
    bkv_init_t   l_init;
    bkv_create_t l_bkv_create=BKV_CREATE_INIT;
    bkv_t        l_handle;
    bkv_val_t    l_val=BKV_VAL_INIT;
    uint8_t     *l_ptr=NULL;
    int          l_ptrlen;
#define TEST_KEY (1)
    int16_t      l_key=TEST_KEY;
    uint16_t     l_value=32;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_init(&l_init));
    l_bkv_create.create_type=BKV_CREATE_TYPE_WORK_IN_RAM;
    l_bkv_create.mode=0777;
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_create(&l_bkv_create,&l_handle));

    /* create bkv:
       { key : l_value ,
         key+1 : {
           key+2:[
                l_value+1,
                l_value+2,
                l_value+3,
                l_value+4,
                l_value+5
                ]
         },
         key+3:l_value+6
       }
    */

    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_open(l_handle,BKV_NO_KEY));
       /* { key : l_value ,*/
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_u16_add(l_handle,l_key,l_value));
       /* key+1 : { */
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_open(l_handle,l_key+1));
       /* key+2:[ */
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_array_open(l_handle,l_key+2));
       /* l_value+1, */
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_array_u16_add(l_handle,l_value+1));
       /* l_value+2, */
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_array_u16_add(l_handle,l_value+2));
       /* l_value+3, */
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_array_u16_add(l_handle,l_value+3));
       /* l_value+4, */
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_array_u16_add(l_handle,l_value+4));
       /* l_value+5  */
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_array_u16_add(l_handle,l_value+5));
        /* ] */
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_array_close(l_handle));
        /* }, */
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_close(l_handle));
        /* key+3:l_value+6 */
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_u16_add(l_handle,l_key++,l_value+2));
       /* } */
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_kv_map_close(l_handle));

    /* get the head. */
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_get_head(l_handle,&l_ptr,&l_ptrlen));
    CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_init(&l_val,l_ptr));

    /* do test the array. */
    { 
        uint16_t     l_search_array [4] = { TEST_KEY+1, TEST_KEY+2, BKV_KEY_INVALID };
        bkv_val_t    l_out_value;
        bkv_val_t    l_array_out_value[5];
        int          l_i;
        CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_val,
                                                  &l_search_array[0],
                                                  1,
                                                  &l_out_value));


        CU_ASSERT_EQUAL_FATAL(BKV_VAL_TYPE_ARRAY,l_out_value.type);
        CU_ASSERT_EQUAL_FATAL(5,l_out_value.u.array.len);
/*        CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_init(&l_array_out_value[0],l_out_value.priv));*/
        /* get the array elements. */
        CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_get2(&l_out_value,
                                                  NULL,
                                                  5,
                                                  &l_array_out_value[0]));

        for (l_i=0;l_i<5;l_i++){
            CU_ASSERT_EQUAL_FATAL(BKV_VAL_TYPE_INT16,l_array_out_value[l_i].type);
            CU_ASSERT_EQUAL_FATAL(l_value+l_i+1,l_array_out_value[l_i].u.number.int16);

        }
        CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_val_rel(&l_out_value));
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
    if (NULL == CU_add_test(pSuite, "search in basic tree", test_bkv_uc03_deep_search)) {
        return CU_get_error();
    }

    if (NULL == CU_add_test(pSuite, "search an array in basic tree", test_bkv_uc03_deep_array_search)) {
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_set_output_filename("cunit_bkv_uc03");
    CU_automated_run_tests();
    num_of_failures = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return num_of_failures;
}
