#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "CUnit/Automated.h"

#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>
#include "bkv_dico.h"

static int init_suite(void){
    return 0;
}

static int clean_suite(void){
    return 0;
}


static void test_bkv_dico_uc00(void){
    bkv_dico_create_t l_create=BKV_DICO_CREATE_INIT;
    bkv_dico_t        l_dico_handle;

    CU_ASSERT_EQUAL(BKV_INV_ARG,bkv_dico_create(NULL,&l_dico_handle));
    CU_ASSERT_EQUAL(BKV_INV_ARG,bkv_dico_create(&l_create,NULL));
}

static void test_bkv_dico_uc01(void){
    bkv_dico_create_t l_create=BKV_DICO_CREATE_INIT;
    bkv_dico_t        l_dico_handle;

    l_create.type=BKV_DICO_TYPE_DIRECT;
    CU_ASSERT_EQUAL(BKV_OK,bkv_dico_create(&l_create,&l_dico_handle));
}

static void test_bkv_dico_uc02(void){
    bkv_dico_create_t l_create=BKV_DICO_CREATE_INIT;
    bkv_dico_t        l_dico_handle;

    l_create.type=BKV_DICO_TYPE_CACHED;
    CU_ASSERT_EQUAL(BKV_OK,bkv_dico_create(&l_create,&l_dico_handle));
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
    if (NULL == CU_add_test(pSuite, "test_bkv_dico_uc00", test_bkv_dico_uc00)) {
        return CU_get_error();
    }


    if (NULL == CU_add_test(pSuite, "test_bkv_dico_uc01", test_bkv_dico_uc01)) {
        return CU_get_error();
    }

    if (NULL == CU_add_test(pSuite, "test_bkv_dico_uc02", test_bkv_dico_uc02)) {
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_set_output_filename("cunit_bkv_uc01");
    CU_automated_run_tests();
    num_of_failures = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return num_of_failures;
}
