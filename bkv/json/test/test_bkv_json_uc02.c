
#include <stdio.h>
#include "bkv_json_tools.h"
#include "CUnit/Automated.h"

static int init_suite(void){
    return 0;
}

static int clean_suite(void){
    return 0;
}

static void test_bkv_json_tools_compare(void){
    const char *str1= " { \"init\" : 0 } ";
    const char *str2= " { \"init\" : 0, \"term\" : { \"ok\" : 1, \"none\":[ 4.324,-10 ] } } ";
    CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)str1,strlen(str1),(const uint8_t *)str1,strlen(str2)));
    CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)str2,strlen(str2),(const uint8_t *)str2,strlen(str2)));
    CU_ASSERT_NOT_EQUAL(-1,bkv_json_tools_compare((const uint8_t *)str1,strlen(str1),(const uint8_t *)str2,strlen(str2)));
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
    if (NULL == CU_add_test(pSuite, "test_bkv_u16", test_bkv_json_tools_compare)) {
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_set_output_filename("cunit_bkv_uc01");
    CU_automated_run_tests();
    num_of_failures = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return num_of_failures;
}
