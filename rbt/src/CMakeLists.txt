
set(RBT_SRCS
    misc.c
    red_black_tree.c
    stack.c)

set(RBT_LIB rbt)
add_definitions(-DMODULE=\"rbt\")
add_library(${RBT_LIB} SHARED 
	${RBT_SRCS}
    )
target_include_directories(${RBT_LIB} PUBLIC ../inc)

