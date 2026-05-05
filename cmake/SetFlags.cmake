if(MSVC)
  add_compile_options(/fp:fast)

  if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    add_compile_options(
      "$<$<CONFIG:Debug>:/w;/Od;/MTd;/ZI>"
      "$<$<CONFIG:Release>:/Oi;/Gy;/W3;/MT;/Zi>" /permissive- /MP
      ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT} ${DEFAULT_CXX_EXCEPTION_HANDLING})
    add_link_options(
      "$<$<CONFIG:Debug>:/INCREMENTAL>"
      "$<$<CONFIG:Release>:/OPT:REF;/OPT:ICF;/INCREMENTAL:NO;/FORCE:MULTIPLE>"
      /MANIFEST:NO /DEBUG /SUBSYSTEM:WINDOWS)
  elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    add_compile_options(
      "$<$<CONFIG:Debug>:/MTd>"
      "$<$<CONFIG:Release>:/O2;/Oi;/Gy;/MT>"
      /permissive-
      /MP
      /w
      ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
      ${DEFAULT_CXX_EXCEPTION_HANDLING})
    add_link_options(
      "$<$<CONFIG:Debug>:/STACK:8777216>"
      "$<$<CONFIG:Release>:/OPT:REF;/OPT:ICF;/INCREMENTAL:NO;/FORCE:MULTIPLE>"
      /MANIFEST:NO /DEBUG /SUBSYSTEM:WINDOWS)
  endif()

  # Remove /RTC from msvc flags
  foreach(fentry CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
    string(REGEX REPLACE "/RTC(su|[1su])" "" ${fentry} "${${fentry}}")
  endforeach()
else()
  add_compile_options(
    -Wall
    -Wextra
    -Wno-error
    -Wno-missing-field-initializers
    -Wno-parentheses
    -Wno-error=int-conversion
    -Wno-missing-braces
    -ffast-math
    -flto=auto
    -pipe)
  add_link_options(-flto=auto)

  set(C_FLAGS -Werror-implicit-function-declaration
              -Wno-incompatible-pointer-types)
  add_compile_options("$<$<COMPILE_LANGUAGE:C>:${C_FLAGS}>")

  set(CXX_FLAGS -fpermissive -fomit-frame-pointer -Wno-error=narrowing)
  add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${CXX_FLAGS}>")

  add_compile_options(
    "$<$<CONFIG:Debug>:-g>" "$<$<CONFIG:Release>:-O3>"
    "$<$<CONFIG:MinSizeRel>:-Os>" "$<$<CONFIG:RelWithDebInfo>:-O2;-g>")

  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag("-pthread" HAS_PTHREAD)
  if(HAS_PTHREAD AND NOT CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    add_compile_options(-pthread)
    add_link_options(-pthread)
  endif()

  if(NOT CMAKE_SYSTEM_NAME STREQUAL "Darwin"
     AND NOT CMAKE_SYSTEM_NAME STREQUAL "iOS"
     AND NOT CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch"
     AND NOT CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
      check_cxx_compiler_flag("-msse2 -mfpmath=sse" HAS_SSE2)
      if(HAS_SSE2)
        add_compile_options(-msse2 -mfpmath=sse)
      endif()
    endif()

    include(CheckLinkerFlag)
    check_linker_flag("CXX" "-Wl,-export-dynamic" HAS_EXPORT_DYNAMIC)
    if(HAS_EXPORT_DYNAMIC)
      add_link_options(-Wl,-export-dynamic)
    endif()
  endif()
endif()

# Add compile definitions for the target
add_compile_definitions(
  NDEBUG
  VERSION_US=1
  "$<$<BOOL:${USE_OPENGLES}>:USE_OPENGLES>"
  ENABLE_RUMBLE=1
  F3DEX_GBI=1
  _LANGUAGE_C
  _USE_MATH_DEFINES
  CIMGUI_DEFINE_ENUMS_AND_STRUCTS
  NON_MATCHING=1
  NON_EQUIVALENT=1
  AVOID_UB=1
  SPAGHETTI_VERSION="${PROJECT_VERSION}")

add_compile_definitions("$<$<CONFIG:Debug>:_DEBUG>")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  add_compile_definitions(
    "$<$<CONFIG:Debug>:ENABLE_DX11>"
    INCLUDE_GAME_PRINTF
    NOMINMAX
    UNICODE
    _UNICODE
    _CRT_SECURE_NO_WARNINGS
    _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
    STORMLIB_NO_AUTO_LINK)
  set(STORMLIB_NO_AUTO_LINK ON)
elseif(CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
  add_compile_definitions(SPDLOG_ACTIVE_LEVEL=3 SPDLOG_NO_THREAD_ID
                          SPDLOG_NO_TLS STBI_NO_THREAD_LOCALS)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU|Clang|AppleClang")
  add_compile_definitions(
    "$<$<BOOL:${BUILD_CROWD_CONTROL}>:ENABLE_CROWD_CONTROL>"
    SPDLOG_ACTIVE_LEVEL=0 _CONSOLE _CRT_SECURE_NO_WARNINGS UNICODE _UNICODE)
endif()


