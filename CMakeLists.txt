cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# Set the project version and language
project(
  Spaghettify
  VERSION 1.0.0
  LANGUAGES C CXX ASM)
include(FetchContent)

# fmt's consteval format-string checking fails to compile under newer
# AppleClang (Xcode 16+) with the spdlog/fmt versions in use (both vcpkg's on
# CI and Torch's bundled copy — see cmake/TorchExternalFixes.cmake for the
# latter):
#   error: call to consteval function 'fmt::basic_format_string<...>' is not
#   a constant expression
# Defining FMT_CONSTEVAL as empty falls back to fmt's pre-C++20 constexpr
# checking, which compiles cleanly with identical runtime behavior. Scoped to
# AppleClang; other platforms and compilers are untouched.
# TODO: Temporary hack. Remove this (plus cmake/TorchExternalFixes.cmake and
# the vcpkg.json fmt/spdlog pins) once the Torch pin is bumped past Torch's
# spdlog/fmt update. Note the bump also needs matching MK64 loader changes,
# see the discussion on PR #712.
if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_definitions("FMT_CONSTEVAL=")
endif()

# Set Project Version
include(cmake/GetVersion.cmake)

# Set some Cmake variable
include(cmake/SetCmakeVar.cmake)

# add_compile_options(-fsanitize=address) add_link_options(-fsanitize=address)

set(YAML_CPP_STATIC_DEFINE ON)
add_compile_definitions(YAML_CPP_STATIC_DEFINE)
include(cmake/lus-cvars.cmake)

if(WIN32)
  option(ENABLE_VCPKG "Enable vcpkg for dependencies" ON)
else()
  option(ENABLE_VCPKG "Enable vcpkg for dependencies" OFF)
endif()

if(ENABLE_VCPKG)
  include(cmake/dependencies/vcpkg.cmake)
endif()

if(IOS)
  set(PLATFORM "OS64")
  include(cmake/ios.toolchain.cmake)
  add_compile_definitions(PLATFORM_IOS=1)
  set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "YOUR_TEAM_ID")
  set(PROJECT_ID "dev.net64.game")
endif()

# Toggle for GLES (Required for many ARM/Mobile platforms)
option(USE_OPENGLES "Use OpenGLES instead of Desktop OpenGL" OFF)

# Set game compilation version
set(VERSION us)
set(SKIP_XCODE_VERSION_CHECK ON)

# TODO: Sorry i broke this
set(GFX_DEBUG_DISASSEMBLER OFF)

if(MSVC)
  set(CPP "${CMAKE_C_COMPILER}" "/EP")
else()
  set(CPP "${CMAKE_C_COMPILER}" "-E" "-P" "-Wno-trigraphs" "-x" "c")
endif()

include(cmake/SetFlags.cmake)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/properties.h.in
                 ${CMAKE_CURRENT_SOURCE_DIR}/properties.h @ONLY)
endif()

# Include directories
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/assets
  ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Collect source files to build the executable
file(
  GLOB ALL_FILES_ROOT
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "src/*.c"
  "src/*.h"
  "src/os/guLookAtF.c"
  "src/os/guPerspectiveF.c"
  "src/os/guMtxF2L.c"
  "src/os/guRotateF.c"
  "src/os/guOrthoF.c"
  "src/os/guScaleF.c"
  "src/os/guTranslateF.c"
  "src/os/guMtxCatF.c"
  "src/os/guMtxCatL.c"
  "src/os/guNormalize.c")

file(
  GLOB_RECURSE ALL_FILES
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "src/buffers/*.c"
  "src/data/*.c"
  "src/debug/*.c"
  "src/ending/*.c"
  "src/racing/*.c"
  "src/audio/*.c"
  "src/port/*.c"
  "src/port/*.cpp"
  "src/mods/*.c"
  "src/mods/*.cpp"
  "assets/code/*.c"
  "courses/*.c"
  "src/engine/*.cpp"
  "src/engine/tracks/*.c"
  "src/engine/actors/*.cpp"
  "src/engine/objects/*.cpp"
  "src/engine/sky/*.cpp"
  "src/engine/particles/*.cpp"
  "src/enhancements/*.c"
  "src/enhancements/freecam/*.c"
  "src/enhancements/freecam/*.cpp"
  "src/engine/*.cpp"
  "Resource.rc")

list(APPEND ALL_FILES ${ALL_FILES_ROOT})

# Exclude specific files from the ALL_FILES list
list(FILTER ALL_FILES EXCLUDE REGEX ".*.inc.c")
list(FILTER ALL_FILES EXCLUDE REGEX "./src/debug/crash_screen_enhancement.c")

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
  set(IOS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/ios)

  set(STORYBOARD_FILE ${IOS_DIR}/Launch.storyboard)
  set(IMAGE_FILES ${IOS_DIR}/PoweredBy.png)
  set(ICON_FILES ${IOS_DIR}/Icon.png)

  list(APPEND ALL_FILES ${STORYBOARD_FILE} ${IMAGE_FILES} ${ICON_FILES})

  add_executable(${PROJECT_NAME} ${ALL_FILES})
  set_xcode_property(${PROJECT_NAME} PRODUCT_BUNDLE_IDENTIFIER ${PROJECT_ID}
                     All)
  set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES MACOSX_BUNDLE TRUE
               MACOSX_BUNDLE_INFO_PLIST ${IOS_DIR}/plist.in
               RESOURCE "${IMAGE_FILES};${STORYBOARD_FILE};${ICON_FILES}")
else()
  add_executable(${PROJECT_NAME} ${ALL_FILES})
endif()

# ##############################################################################
# MSVC runtime library
# ##############################################################################
if(MSVC)
  if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    get_property(
      MSVC_RUNTIME_LIBRARY_DEFAULT
      TARGET ${PROJECT_NAME}
      PROPERTY MSVC_RUNTIME_LIBRARY)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
      string(
        CONCAT
          "MSVC_RUNTIME_LIBRARY_STR"
          $<$<CONFIG:Debug>:
          MultiThreadedDebug
          >
          $<$<CONFIG:Release>:
          MultiThreaded
          >
          $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
      )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
      string(
        CONCAT
          "MSVC_RUNTIME_LIBRARY_STR"
          $<$<CONFIG:Debug>:
          MultiThreadedDebug
          >
          $<$<CONFIG:Release>:
          MultiThreaded
          >
          $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
      )
    endif()
    set_target_properties(
      ${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY
                                 ${MSVC_RUNTIME_LIBRARY_STR})
  endif()
endif()

include(cmake/dependencies/common.cmake)

include(cmake/dependencies/FindLib.cmake)


if(CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch")

  nx_generate_nacp(
    ${PROJECT_NAME}.nacp
    NAME
    "${PROJECT_NAME}"
    AUTHOR
    "${PROJECT_TEAM}"
    VERSION
    "${PROJECT_VERSION}")

  nx_create_nro(${PROJECT_NAME} NACP ${PROJECT_NAME}.nacp ICON
                ${CMAKE_CURRENT_SOURCE_DIR}/icon.png)

  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.nro
    DESTINATION .
    COMPONENT ${PROJECT_NAME})
endif()

# ##############################################################################
# Compile and link options
# ##############################################################################


add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMENT "Copying asset yamls..."
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/config.yml"
          "$<TARGET_FILE_DIR:Spaghettify>/config.yml"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/yamls/"
          "$<TARGET_FILE_DIR:Spaghettify>/yamls/"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/meta/"
          "$<TARGET_FILE_DIR:Spaghettify>/meta/")

if(NOT CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
  include(ExternalProject)
  # Torch's pinned spdlog bundles an fmt that trips a consteval error under
  # newer AppleClang (Xcode 16+): "call to consteval function
  # 'fmt::basic_format_string<...>' is not a constant expression". Neutralize
  # FMT_CONSTEVAL inside the Torch sub-build via CMAKE_PROJECT_INCLUDE (Torch's
  # CMakeLists overwrites CMAKE_CXX_FLAGS, so flags can't be injected that
  # way). Fixes the build-macos-arm64 job; the intel runner's older Xcode
  # doesn't hit it.
  # TODO: Temporary hack. Remove the CMAKE_PROJECT_INCLUDE line below (plus
  # cmake/TorchExternalFixes.cmake itself) once the Torch pin is bumped past
  # Torch's spdlog/fmt update, see the discussion on PR #712.
  ExternalProject_Add(TorchExternal
    PREFIX TorchExternal
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/torch
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/torch
               -DCMAKE_PROJECT_INCLUDE=${CMAKE_SOURCE_DIR}/cmake/TorchExternalFixes.cmake
)

  ExternalProject_Get_Property(TorchExternal install_dir)
  if(MSVC)
    set(TORCH_EXECUTABLE
        ${install_dir}/src/TorchExternal-build/$<CONFIGURATION>/torch)
  else()
    set(TORCH_EXECUTABLE ${install_dir}/src/TorchExternal-build/torch)
  endif()

  add_custom_target(
    ExtractAssets
    DEPENDS TorchExternal
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${TORCH_EXECUTABLE} header -o baserom.us.z64
    COMMAND ${TORCH_EXECUTABLE} o2r baserom.us.z64 --additional-files
            meta/mods.toml
    COMMAND ${TORCH_EXECUTABLE} pack assets spaghetti.o2r o2r
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/mk64.o2r"
            "${CMAKE_BINARY_DIR}/mk64.o2r"
    COMMAND
      ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/spaghetti.o2r"
      "${CMAKE_BINARY_DIR}/spaghetti.o2r")

  add_custom_target(
    GenerateO2R
    DEPENDS TorchExternal
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${TORCH_EXECUTABLE} pack assets spaghetti.o2r o2r
    COMMAND
      ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/spaghetti.o2r"
      "${CMAKE_BINARY_DIR}/spaghetti.o2r")

  if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    install(
      FILES "${CMAKE_BINARY_DIR}/spaghetti.o2r"
      DESTINATION .
      COMPONENT ${PROJECT_NAME})
  endif()

  if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    add_custom_target(
      CreateOSXIcons
      COMMAND mkdir -p ${CMAKE_BINARY_DIR}/macosx/spaghettikart.iconset
      COMMAND sips -z 16 16 icon.png --out
              ${CMAKE_BINARY_DIR}/macosx/spaghettikart.iconset/icon_16x16.png
      COMMAND sips -z 32 32 icon.png --out
              ${CMAKE_BINARY_DIR}/macosx/spaghettikart.iconset/icon_16x16@2x.png
      COMMAND sips -z 32 32 icon.png --out
              ${CMAKE_BINARY_DIR}/macosx/spaghettikart.iconset/icon_32x32.png
      COMMAND sips -z 64 64 icon.png --out
              ${CMAKE_BINARY_DIR}/macosx/spaghettikart.iconset/icon_32x32@2x.png
      COMMAND sips -z 128 128 icon.png --out
              ${CMAKE_BINARY_DIR}/macosx/spaghettikart.iconset/icon_128x128.png
      COMMAND
        sips -z 256 256 icon.png --out
        ${CMAKE_BINARY_DIR}/macosx/spaghettikart.iconset/icon_128x128@2x.png
      COMMAND sips -z 256 256 icon.png --out
              ${CMAKE_BINARY_DIR}/macosx/spaghettikart.iconset/icon_256x256.png
      COMMAND
        sips -z 512 512 icon.png --out
        ${CMAKE_BINARY_DIR}/macosx/spaghettikart.iconset/icon_256x256@2x.png
      COMMAND sips -z 512 512 icon.png --out
              ${CMAKE_BINARY_DIR}/macosx/spaghettikart.iconset/icon_512x512.png
      COMMAND
        cp icon.png
        ${CMAKE_BINARY_DIR}/macosx/spaghettikart.iconset/icon_512x512@2x.png
      COMMAND iconutil -c icns -o ${CMAKE_BINARY_DIR}/macosx/spaghettikart.icns
              ${CMAKE_BINARY_DIR}/macosx/spaghettikart.iconset
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "Creating OSX icons ...")
    add_dependencies(${PROJECT_NAME} CreateOSXIcons)
    configure_file("${CMAKE_SOURCE_DIR}/Info.plist"
                   "${CMAKE_BINARY_DIR}/Info.plist" COPYONLY)
  endif()

  set_property(TARGET ${PROJECT_NAME} PROPERTY APPIMAGE_DESKTOP_FILE_TERMINAL
                                               YES)
  set_property(
    TARGET ${PROJECT_NAME} PROPERTY APPIMAGE_DESKTOP_FILE
                                    "${CMAKE_SOURCE_DIR}/SpaghettiKart.desktop")
  set_property(TARGET ${PROJECT_NAME} PROPERTY APPIMAGE_ICON_FILE
                                               "${CMAKE_SOURCE_DIR}/icon.png")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(CPACK_GENERATOR "External")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows|NintendoSwitch|CafeOS")
  set(CPACK_GENERATOR "ZIP")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set(CPACK_GENERATOR "Bundle")
endif()

set(CPACK_PROJECT_CONFIG_FILE
    ${CMAKE_SOURCE_DIR}/cmake/configure-packaging.cmake)
include(cmake/packaging.cmake)
