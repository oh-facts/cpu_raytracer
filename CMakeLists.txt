cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(yk2)

# to disable stupid msvc errors
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

#--------target--------#

add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

include_directories(
    src

    vendor/SDL/include
    vendor/miniaudio
	vendor/stb
)

# engine code
set(yke "src/yk_renderer.c" "src/stb_truetype.c")

# game code
set(ykg "src/yk_game.c")

# game + engine built as dll
add_library(yk2 SHARED ${ykg} ${yke}) 

# platform code
set(ykp "src/platform/yk_platform_sdl.c" "src/platform/miniaudio.c" "src/platform/yk_platform.c")

# platform is entry
add_executable(yk_test  ${ykp})

if(WIN32)
    target_link_libraries(yk_test SDL2main SDL2)
endif()

if(UNIX)
    # need to link with m on unix for miniaudio to compile
    target_link_libraries(yk_test SDL2main SDL2  m)
endif()

#----------copy vendor dlls to bin folder----------#

if(WIN32)
    add_custom_target(
        copy-dlls ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/vendor/SDL/SDL2d.dll
            ${CMAKE_BINARY_DIR}
        DEPENDS yk_test
    )
elseif(APPLE)
    # Note(facts): Haven't tested this. I am assuming it is called libSDL2d.dylib
    # It is probably called something else since sdl2 can't make up their mind on
    # what to call their libraries
    add_custom_target(
        copy-dylibs ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/vendor/SDL/libSDL2d.dylib
            ${CMAKE_BINARY_DIR}
        DEPENDS yk_test
    )
elseif(UNIX)
    add_custom_target(
        copy-so ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/vendor/SDL/libSDL2-2.0.so
            ${CMAKE_BINARY_DIR}
        DEPENDS yk_test
    )
endif()


#-------------------------------------------------------------------#