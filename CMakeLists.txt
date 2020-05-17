cmake_minimum_required(VERSION 3.15)
project(strange LANGUAGES C CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(SAIS_SRC
        src/cards.cpp
        src/combat.cpp
        src/combat_display.cpp
        src/combat_init.cpp
        src/combat_sim.cpp
        src/combat_weapons.cpp
        src/endgame.cpp
        src/font.cpp
        src/gfx.cpp
        src/interface.cpp
        src/is_fileio.cpp
        src/main.cpp
        src/modconfig.cpp
        src/physfsrwops.c
        src/sdl_iface.cpp
        src/sdl_main.cpp
        src/sound.cpp
        src/sprite_init.cpp
        src/sprites.cpp
        src/starmap.cpp
        src/starmap_encounters.cpp
        src/starmap_init.cpp
        src/starmap_inventory.cpp
        src/startgame.cpp
        src/textstr.cpp
        src/w32_gfx.cpp
        src/w32_sound.cpp)

add_executable(strangedemo WIN32 ${SAIS_SRC})
target_link_libraries(strangedemo
        PRIVATE
        CONAN_PKG::sdl2
        CONAN_PKG::sdl2_mixer
        CONAN_PKG::physfs)
target_compile_definitions(strangedemo PRIVATE DEMO_VERSION=1)
add_executable(strange WIN32 ${SAIS_SRC})
target_link_libraries(strange
        PRIVATE
        CONAN_PKG::sdl2
        CONAN_PKG::sdl2_mixer
        CONAN_PKG::physfs)

if(MSVC)
    target_sources(strangedemo PRIVATE
            src/Script1.rc)
    target_sources(strange PRIVATE
        src/Script1.rc)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1)
endif()