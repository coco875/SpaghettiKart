include(CheckCXXCompilerFlag)
include(CheckLinkerFlag)

check_cxx_compiler_flag("-pthread" HAS_PTHREAD)
if(HAS_PTHREAD)
  add_compile_options(-pthread)
  add_link_options(-pthread)
endif()
