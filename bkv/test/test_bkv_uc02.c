#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "CUnit/Automated.h"

#include "bkv.h"
#include "bkv_byteorder.h"
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
}

static void test_bkv_serialize_float(void){
    uint8_t b[10];
    float start=4.353;
    float v;
    printf("float : %x %x %x %x\n",
           ((uint8_t*)&start)[0],
           ((uint8_t*)&start)[1],
           ((uint8_t*)&start)[2],
           ((uint8_t*)&start)[3]);
    printf("float %x\n",*(int32_t*)&start);

    cpu_to_lef(start,&b[0],NULL);
    lef_to_cpu(&b[0],&v,NULL);
    printf ("%f \n",v);
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
    if (NULL == CU_add_test(pSuite, "test_bkv_init_term", test_bkv_serialize_float)) {
        return CU_get_error();
    }
    /* Run all tests using the CUnit Basic interface */
    CU_set_output_filename("cunit_bkv_uc02");
    CU_automated_run_tests();
    num_of_failures = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return num_of_failures;
}
