#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include "bkv_json_tools.h"
#include "bkv_json_yajl.h"
#include "bkv_json.h"
#include "bkv_print.h"
#include "bkv.h"
#include "CUnit/Automated.h"

typedef struct {
    const char *tag;
    const char *url;
} json_web_sources_t;

static int init_suite(void){
    return 0;
}

static int clean_suite(void){
    return 0;
}

static void test_bkv_json_dico_challenge(void){
    bkv_from_json_parser_t  from=bkv_from_json_yajl_parser_get();
    bkv_to_json_parser_t    to=bkv_to_json_yajl_parser_get();
    int                     l_i;
    uint8_t                *out_str;
    int                     out_str_len;
    uint8_t                *str[2]={ 
        (uint8_t*)"{\"i\":[{\"jjjjjjjjjjjjjjjj\":0},{\"jjjjjjjjjjjjjjjj\":1}]}",
        (uint8_t*)"\
            {\
  \"markers\": [\
    {\
      \"name\": \"Rixos The Palm Dubai\",\
      \"position\": [25.1212, 55.1535]\
    },\
    {\
      \"name\": \"Shangri-La Hotel\",\
      \"location\": [25.2084, 55.2719]\
    },\
    {\
      \"name\": \"Grand Hyatt\",\
      \"location\": [25.2285, 55.3273]\
    }\
  ]\
}\
            "};
    bkv_t                   h;

    for (l_i=0;l_i<(int)(sizeof(str) / sizeof(str[0])); l_i++){
        CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_from_json(BKV_DICO_TYPE_CACHED,
                                                   from,(uint8_t*)str[l_i],
                                                   strlen((char*)str[l_i]),&h));
        CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_to_json(to,h,&out_str,&out_str_len));
        {
            uint8_t *l_raw;
            int      l_raw_len;
            bkv_get_head(h,&l_raw,&l_raw_len);
            printf("LLEN %d %d\n",l_raw_len,out_str_len);
        }

        bkv_print2(h);
        CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)str[l_i],strlen((char*)str[l_i]),(const uint8_t*)out_str,out_str_len));
    }

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
    if (NULL == CU_add_test(pSuite, "test_bkv_u16", test_bkv_json_dico_challenge)) {
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_set_output_filename("cunit_bkv_json_uc04");
    CU_automated_run_tests();
    num_of_failures = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return num_of_failures;
}
