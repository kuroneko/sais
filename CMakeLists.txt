cmake_minimum_required(VERSION 3.15)

# This enables the new MSVC runtime library selection mechanism.
cmake_policy(SET CMP0091 NEW)

# Whislt we're not using C++14 much in the codebase, that is our assumed target.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED 14)

project(strange VERSION 1.6.0 LANGUAGES C CXX)

option(USE_CONAN "Use conan for dependencies" OFF)

# The demonstration version of SAIS is only kept for historical reference, and
# is not the default, ever.
option(BUILD_DEMO "Build the Demo Version of SAIS" OFF)

# This relates to SAIS's original interna logging, not the GPL version's SDL Logging which is always on.
option(USE_LOG "Enable game logging" OFF)

if(USE_CONAN)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
    set(SAIS_LIBS CONAN_PKG::sdl2 CONAN_PKG::sdl2_mixer CONAN_PKG::physfs)
else()
    include(FindPkgConfig)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2MIXER REQUIRED SDL2_mixer)

    # physfs does not reliably have a pkg-config file - use path searching to find its bits.
    find_library(PHYSFS NAMES physfs PATH_SUFFIXES physfs lib)
    find_path(PHYSFS_INCLUDE_DIR NAMES physfs.h PATH_SUFFIXES physfs include Headers)

    set(SAIS_LIBS ${SDL2_LIBRARIES} ${SDL2MIXER_LIBRARIES} ${PHYSFS})
    set(SAIS_LIB_DIRS ${SDL2_LIBRARY_DIRS} ${SDL2MIXER_LIBRARY_DIRS})
    set(SAIS_INCLUDES ${SDL2_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS} ${PHYSFS_INCLUDE_DIR})
endif()

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
    src/sprites.cpp
    src/starmap.cpp
    src/starmap_encounters.cpp
    src/starmap_init.cpp
    src/starmap_inventory.cpp
    src/startgame.cpp
    src/textstr.cpp
    src/w32_gfx.cpp
    src/w32_sound.cpp)

if(BUILD_DEMO)
    add_executable(strangedemo WIN32 MACOSX_BUNDLE ${SAIS_SRC})
    target_link_directories(strangedemo PRIVATE ${SAIS_LIB_DIRS})
    target_link_libraries(strangedemo
        PRIVATE
        ${SAIS_LIBS})
    target_include_directories(strangedemo PRIVATE ${SAIS_INCLUDES})
    target_compile_definitions(strangedemo PRIVATE DEMO_VERSION=1)

    install(TARGETS strangedemo RUNTIME DESTINATION . BUNDLE DESTINATION .)
    install(DIRECTORY demo/ DESTINATION .)

    add_subdirectory(demo-data)
else()
    add_executable(strange WIN32 MACOSX_BUNDLE ${SAIS_SRC})
    target_link_directories(strange PRIVATE ${SAIS_LIB_DIRS})
    target_link_libraries(strange
        PRIVATE
        ${SAIS_LIBS})
    target_include_directories(strange PRIVATE ${SAIS_INCLUDES})
    install(TARGETS strange RUNTIME DESTINATION . BUNDLE DESTINATION .)
    install(DIRECTORY full/ DESTINATION .)

    add_subdirectory(full-data)
endif()

# MSVC Specific bits:
#
#  * Include the resource script which sets our icon.
#  * Force the runtime type to MT(d).
#  * Supresss the MSVC "Insecure C Runtime" warnings which are largely relating
#    to ANSI C functions.
#
if(MSVC)
    if(BUILD_DEMO)
        target_sources(strangedemo PRIVATE
            src/Script1.rc)
        set_property(TARGET strangedemo PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        target_sources(strange PRIVATE
            src/Script1.rc)
        set_property(TARGET strange PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()

    add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1)
endif()

# macOS Specific bits:
#
# * Set up the macOS bundle details.
# * Embed the game data into the bundle.
# * Ensure we build the game data before the executable.
if(CMAKE_SYSTEM_NAME MATCHES Darwin)
    if(BUILD_DEMO)
        set_target_properties(strangedemo PROPERTIES
                OUTPUT_NAME "Strange Adventures in Infinite Space Demo"
                MACOSX_FRAMEWORK_IDENTIFIER "au.com.ecsim.saisgpldemo"
                MACOSX_BUNDLE TRUE
                MACOSX_BUNDLE_BUNDLE_NAME "Strange Adventures Demo"
                MACOSX_BUNDLE_COPYRIGHT "Copyright © 2020 Chris Collins, Copyright © 2005 Richard Carlson, Iikka Keranen and William Sears"
                MACOSX_BUNDLE_ICON_FILE "sais.icns"
                XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
        target_sources(strangedemo PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/demo-data/saisdemo.zip" "src/sais.icns")
        set_property(TARGET strangedemo PROPERTY
                RESOURCE "${CMAKE_CURRENT_BINARY_DIR}/demo-data/saisdemo.zip;src/sais.icns")
        set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/demo-data/saisdemo.zip" PROPERTY GENERATED TRUE)
        add_dependencies(strangedemo demo_data)
    else()
        set_target_properties(strange PROPERTIES
                OUTPUT_NAME "Strange Adventures in Infinite Space"
                MACOSX_FRAMEWORK_IDENTIFIER "au.com.ecsim.saisgpl"
                MACOSX_BUNDLE TRUE
                MACOSX_BUNDLE_BUNDLE_NAME "Strange Adventures"
                MACOSX_BUNDLE_COPYRIGHT "Copyright © 2020 Chris Collins, Copyright © 2005 Richard Carlson, Iikka Keranen and William Sears"
                MACOSX_BUNDLE_ICON_FILE "sais.icns"
                XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME TRUE)
        target_sources(strange PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/full-data/saisdata.zip" "src/sais.icns")
        set_property(TARGET strange PROPERTY
                RESOURCE "${CMAKE_CURRENT_BINARY_DIR}/full-data/saisdata.zip;src/sais.icns")
        set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/full-data/saisdata.zip" PROPERTY GENERATED TRUE)
        add_dependencies(strange retail_data)
    endif()
endif()

if(USE_LOG)
    add_compile_definitions(LOG_OUTPUT=1)
endif()

if(NOT BUILD_DEMO)
    set(CPACK_WIX_UPGRADE_GUID E709D29C-1BD8-4679-A8CA-9D40567771F5)
    if(CMAKE_SYSTEM_NAME MATCHES Linux)
        set(CPACK_GENERATOR TGZ)
        set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
    elseif(CMAKE_SYSTEM_NAME MATCHES Windows)
        set(CPACK_GENERATOR ZIP;WIX)
        set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
    elseif(CMAKE_SYSTEM_NAME MATCHES Darwin)
        set(CPACK_GENERATOR ZIP;DragNDrop)
        set(CPACK_SYSTEM_NAME macOS-${CMAKE_SYSTEM_PROCESSOR})
    endif()
    set(CPACK_PACKAGE_NAME  "Strange Adventures in Infinite Space GPL")
    set(CPACK_PACKAGE_FILE_NAME "SAIS-GPL-${CMAKE_PROJECT_VERSION}-${CPACK_SYSTEM_NAME}")
    set(CPACK_PACKAGE_VENDOR "FreeSAIS")
    set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/kuroneko/sais/")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "SAIS-GPL")
    set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/full/README1st.txt)
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/full/LICENSES.txt)
    set(CPACK_PACKAGE_EXECUTABLES "strange;Strange Adventures in Infinite Space GPL")
    include(CPack)
endif()
