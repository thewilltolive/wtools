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
static json_web_sources_t s_json_web_sources[] = {
    {"map", "https://data.cityofnewyork.us/api/views/kku6-nxdu/rows.json?accessType=DOWNLOAD"}
};

static int init_suite(void){
    return 0;
}

static int clean_suite(void){
    return 0;
}

static int test_load_source(json_web_sources_t  *s,
                            uint8_t            **p_ptr,
                            int                 *p_len){
    uint8_t     l_data[256],*l_ptr;
    struct stat l_statbuf;
    int         l_fd;

    if (access(s->tag,R_OK) !=0){
        snprintf((char*)&l_data[0],sizeof(l_data),"wget -O %s %s",s->tag,s->url);
        system((char*)&l_data[0]);
    }
    if (-1 == (l_fd = open(s->tag,O_RDONLY))){
    }
    else if (-1 == fstat(l_fd,&l_statbuf)){
    }
    else if (MAP_FAILED == (l_ptr = mmap(NULL,l_statbuf.st_size,PROT_READ,MAP_PRIVATE,l_fd,0))){
    }
    else {
        *p_ptr=l_ptr;
        *p_len=l_statbuf.st_size;
        close(l_fd);
    }
    return(0);
}

static void test_bkv_json_test_large_json(void){
    int           l_i,l_len,l_mapped_json_len;
    bkv_t         l_bkv_handle;
    uint8_t      *l_ptr=NULL,*l_mapped_json;

    for (l_i=0;l_i<(int)(sizeof(s_json_web_sources) / sizeof(s_json_web_sources[0]));l_i++){
        test_load_source(&s_json_web_sources[l_i],&l_mapped_json,&l_mapped_json_len);
        CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_from_json(BKV_DICO_TYPE_DIRECT,
                                                   bkv_from_json_yajl_parser_get(),
                                                   l_mapped_json,
                                                   l_mapped_json_len,
                                                   &l_bkv_handle));
        bkv_print2(l_bkv_handle);
        CU_ASSERT_EQUAL_FATAL(BKV_OK,bkv_to_json(bkv_to_json_yajl_parser_get(),
                                                 l_bkv_handle,
                                                 &l_ptr,
                                                 &l_len));
    CU_ASSERT_EQUAL(0,bkv_json_tools_compare((const uint8_t *)l_mapped_json,l_mapped_json_len,(const uint8_t *)l_ptr,l_len));
        printf("OUT %.*s\n",l_len,l_ptr);
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
    if (NULL == CU_add_test(pSuite, "test_bkv_u16", test_bkv_json_test_large_json)) {
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_set_output_filename("cunit_bkv_json_uc03");
    CU_automated_run_tests();
    num_of_failures = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return num_of_failures;
}
