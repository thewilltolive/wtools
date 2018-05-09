set(KEYTREE_LIB kt)
add_definitions(-DMODULE=\"keytree\")
add_library(${KEYTREE_LIB} SHARED 
keytree.c
    )
target_link_libraries(${KEYTREE_LIB} curl )
target_include_directories(${KEYTREE_LIB} PUBLIC ../inc)

