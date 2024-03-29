cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#project name
project(yk2)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

#--------target--------#

if(WIN32)
    add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)
endif()
if(UNIX)
    find_package(SDL2 REQUIRED)
endif()

include_directories(
    src
    src/platform
    vendor/SDL/include
    vendor/miniaudio
    )


set(yk_src  "src/yk_arena.c" "src/yk_game.c" "src/yk_arena.c")

add_library(${PROJECT_NAME} SHARED ${yk_src} ${yk_platform}) 

add_executable(yk_test "src/platform/yk_platform_sdl.c" "vendor/miniaudio/miniaudio.c")

if(WIN32)
    target_link_libraries(yk_test yk2 SDL2main SDL2)
endif()

if(UNIX)
    target_link_libraries(yk_test yk2 SDL2main SDL2 m)
endif()

#----------copy files----------#

#|---compile-commands-|

add_custom_target(
    yk_copy-compile-commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_LIST_DIR}
)
if(WIN32)
    
    add_custom_target(
        copy-dlls ALL
        ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/vendor/SDL/SDL2d.dll
            ${CMAKE_BINARY_DIR}
    )

    add_dependencies(copy-dlls yk_test)
endif()

#|----------------|

#---------------------#