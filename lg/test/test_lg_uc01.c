#include "lg.h"
#include "CUnit/Automated.h"

static int init_suite(void){
    return 0;
}

static int clean_suite(void){
    return 0;
}


static void test_lw_uc01_tc01(void){
    lg_init_params_t l_lw_init_params=LG_INIT_PARAMS_INIT;

    CU_ASSERT_EQUAL(LG_E_BAD_PARAMETER,lg_init(NULL));
    CU_ASSERT_EQUAL(0,lg_init(&l_lw_init_params));
    CU_ASSERT_EQUAL(0,lg_lib_add("toto"));
    CU_ASSERT_EQUAL(0,lg_term());
}

int main(int argc, char **argv) {
    CU_pSuite pSuite = NULL;
    int num_of_failures = 0;
    (void) argc;
    (void) argv;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite "LIBNAME, init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pSuite, "test_lw_uc01_tc01", test_lw_uc01_tc01)) {
        return CU_get_error();
    }


    /* Run all tests using the CUnit Basic interface */
    CU_set_output_filename("cunit_"LIBNAME"_uc01");
    CU_automated_run_tests();
    num_of_failures = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return num_of_failures;
    return(0);
}
