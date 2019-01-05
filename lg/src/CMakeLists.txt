set(LG_LIB lg)
add_definitions(-DMODULE=\"lg\")
add_library(${LG_LIB} SHARED 
	lg.c
    )
target_include_directories(${LG_LIB} PUBLIC ../inc)

