cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#project name
project(yk2)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

#--------target--------#
 
include_directories(
    src
    src/platform
    vendor/final_platform_layer
    vendor/miniaudio
	vendor/stb
)

set(yk_src  "src/yk_arena.c" "src/yk_game.c" "src/yk_arena.c")

add_library(${PROJECT_NAME} SHARED ${yk_src} ${yk_platform}) 

add_executable(yk_test "src/platform/yk_platform_sdl.c" "vendor/miniaudio/miniaudio.c" "vendor/stb/stb_truetype.c" "vendor/final_platform_layer/final_platform_layer.c")

if(WIN32)
    target_link_libraries(yk_test yk2)
endif()

if(UNIX)
    target_link_libraries(yk_test yk2 m)
endif()

#----------copy files----------#

#|---compile-commands-|

add_custom_target(
    yk_copy-compile-commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_LIST_DIR}
)
#|----------------|

#---------------------#