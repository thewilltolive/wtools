function(declare_c_header headers)
  install(FILES ${headers}
	  DESTINATION inc
	  PERMISSIONS OWNER_READ GROUP_READ)
endfunction()
macro(declare_lib)
  get_filename_component(libname "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
  add_definitions(-DLIBNAME=\"${libname}\")
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-init,${libname}_load_init")
endmacro()
