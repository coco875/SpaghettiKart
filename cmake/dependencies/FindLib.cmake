if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  find_package(Ogg CONFIG REQUIRED)
  link_libraries(Ogg::ogg)

  find_package(Vorbis CONFIG REQUIRED)
  link_libraries(Vorbis::vorbisfile)
  set(ADDITIONAL_LIBRARY_DEPENDENCIES "Ogg::ogg" "Vorbis::vorbis"
                                      "Vorbis::vorbisenc" "Vorbis::vorbisfile")
elseif(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
  set(ADDITIONAL_LIBRARY_DEPENDENCIES -lglad SDL2::SDL2)
elseif(CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
  set(ADDITIONAL_LIBRARY_DEPENDENCIES "$<$<CONFIG:Debug>:-Wl,--wrap=abort>")
  include_directories(${DEVKITPRO}/portlibs/wiiu/include/)
else()
  find_package(Ogg REQUIRED)
  find_package(Vorbis REQUIRED)
  set(ADDITIONAL_LIBRARY_DEPENDENCIES "Ogg::ogg" "Vorbis::vorbis"
                                      "Vorbis::vorbisenc" "Vorbis::vorbisfile")
endif()

if(UNIX AND NOT APPLE)
  if(USE_OPENGLES)
    find_library(GLESv2_LIBRARY GLESv2 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GLESv2_LIBRARY})
  else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
  endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
  find_package(SDL2)
endif()


if(NOT USE_OPENGLES)
  include_directories(${GLEW_INCLUDE_DIRS})
endif()

target_link_libraries(${PROJECT_NAME}
                      PRIVATE torch "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
