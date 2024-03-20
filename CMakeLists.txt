cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#project name
project(yk2)

# to disable stupid msvc errors
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

#--------target--------#

add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)
add_subdirectory(vendor/SDL_ttf)

include_directories(
    src
    vendor/SDL/include
    vendor/miniaudio
	vendor/SDL_ttf/include
)

add_library(${PROJECT_NAME} SHARED "src/yk_game.c") 

add_executable(yk_test "src/platform/yk_platform_sdl.c" "src/platform/yk_platform_miniaudio.c")


if(WIN32)
    target_link_libraries(yk_test SDL2main SDL2 SDL2_ttf)
endif()

if(UNIX)
    target_link_libraries(yk_test SDL2main SDL2 SDL2_ttf  m)
endif()

#----------copy vendor dlls to bin folder (reqd for win32)----------#

if(WIN32)
    add_custom_target(
        copy-dlls ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/vendor/SDL/SDL2d.dll
            ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/vendor/SDL_ttf/SDL2_ttfd.dll
            ${CMAKE_BINARY_DIR}
        DEPENDS yk_test
    )
endif()

#-------------------------------------------------------------------#