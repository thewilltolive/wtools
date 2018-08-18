#include "keytree.h"
#include "CUnit/Automated.h"


static int init_suite(void){
    return 0;
}

static int clean_suite(void){
    return 0;
}

static int s_nb_keys=0;
static w_error_t s_kt_counter(wg_key_t      *p_key,
                              const uint8_t *p_str,
                              int            str_len,
                              void          *p_data){
    (void)p_str;
    (void)str_len;
    (void)p_data;
    CU_ASSERT_PTR_EQUAL(p_data,&s_nb_keys);
    if (p_key->end_of_word){
        if ((strncmp((const char*)p_str,"thestring",str_len) == 0) ||
             (strncmp((const char*)p_str,"theotherstring",str_len) == 0)){
            s_nb_keys++;
        }
    }
    return(W_NO_ERROR);
}

static void test_keytree_uc01_tc01(void){
    keytree_list_t  l_list=KEYTREE_LIST_INIT;
    keytree_head_t *l_key_head=NULL;
    keytree_elem_t  l_elem1 = KEYTREE_ELEM_VALUE("thestring",32);
    keytree_elem_t  l_elem2 = KEYTREE_ELEM_VALUE("theotherstring",32);
    void           *l_priv_data=NULL;
    CU_ASSERT_EQUAL(W_NO_ERROR,wg_keytree_create(&l_list,&l_key_head));
    CU_ASSERT_EQUAL(W_NO_ERROR,wg_keytree_add(l_key_head,&l_elem1,&l_priv_data));
    CU_ASSERT_EQUAL(W_NO_ERROR,wg_keytree_add(l_key_head,&l_elem2,&l_priv_data));
    CU_ASSERT_EQUAL(NULL,l_priv_data);
    CU_ASSERT_EQUAL(W_E_EXISTS,wg_keytree_add(l_key_head,&l_elem1,&l_priv_data));
    CU_ASSERT_EQUAL(32,(size_t)l_priv_data);
    CU_ASSERT_EQUAL(W_NO_ERROR,wg_keytree_foreach(l_key_head,s_kt_counter,&s_nb_keys));
    CU_ASSERT_EQUAL(s_nb_keys,2);
    CU_ASSERT_EQUAL(W_NO_ERROR,wg_keytree_release(l_key_head));
}

static void test_keytree_uc01_tc02(void){
    keytree_list_t  l_list=KEYTREE_LIST_INIT;
    keytree_head_t *l_key_head=NULL;
    keytree_elem_t  l_elem= KEYTREE_ELEM_VALUE("AbCdEfGhIjKlOpQrStUvWxYz",32);
    void           *l_priv_data=NULL;
    CU_ASSERT_EQUAL(W_NO_ERROR,wg_keytree_create(&l_list,&l_key_head));
    CU_ASSERT_EQUAL(W_NO_ERROR,wg_keytree_add(l_key_head,&l_elem,&l_priv_data));
    CU_ASSERT_EQUAL(NULL,l_priv_data);
    CU_ASSERT_EQUAL(W_E_EXISTS,wg_keytree_add(l_key_head,&l_elem,&l_priv_data));
    CU_ASSERT_EQUAL(32,(size_t)l_priv_data);
    CU_ASSERT_EQUAL(W_NO_ERROR,wg_keytree_release(l_key_head));
}

static void test_keytree_uc01_tc03(void){
    keytree_list_t  l_list=KEYTREE_LIST_INIT;
    keytree_head_t *l_key_head=NULL;
    keytree_elem_t  l_elem= KEYTREE_ELEM_VALUE("A0B1c2309",32);
    void           *l_priv_data=NULL;
    CU_ASSERT_EQUAL(W_NO_ERROR,wg_keytree_create(&l_list,&l_key_head));
    CU_ASSERT_EQUAL(W_NO_ERROR,wg_keytree_add(l_key_head,&l_elem,&l_priv_data));
    CU_ASSERT_EQUAL(NULL,l_priv_data);
    CU_ASSERT_EQUAL(W_E_EXISTS,wg_keytree_add(l_key_head,&l_elem,&l_priv_data));
    CU_ASSERT_EQUAL(32,(size_t)l_priv_data);
    CU_ASSERT_EQUAL(W_NO_ERROR,wg_keytree_release(l_key_head));
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
    pSuite = CU_add_suite("Suite bkv_to_json", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pSuite, "test_keytree_uc01_tc01", test_keytree_uc01_tc01)) {
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pSuite, "test_keytree_uc01_tc02", test_keytree_uc01_tc02)) {
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (NULL == CU_add_test(pSuite, "test_keytree_uc01_tc03", test_keytree_uc01_tc03)) {
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_set_output_filename("cunit_kt_uc01");
    CU_automated_run_tests();
    num_of_failures = CU_get_number_of_tests_failed();
    CU_cleanup_registry();
    return num_of_failures;
    return(0);
}
