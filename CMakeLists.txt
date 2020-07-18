cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED 14)
project(strange VERSION 1.6.0 LANGUAGES C CXX)

option(USE_CONAN "Use conan for dependencies" OFF)
option(BUILD_DEMO "Build the Demo Version of SAIS" OFF)
option(USE_LOG "Enable game logging" OFF)

if(USE_CONAN)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
    set(SAIS_LIBS CONAN_PKG::sdl2 CONAN_PKG::sdl2_mixer CONAN_PKG::physfs)
else()
    include(FindPkgConfig)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2MIXER REQUIRED SDL2_mixer)
    pkg_check_modules(PHYSFS REQUIRED physfs)

    set(SAIS_LIBS ${SDL2_LIBRARIES} ${SDL2MIXER_LIBRARIES} ${PHYSFS_LIBRARIES})
    set(SAIS_LIB_DIRS ${SDL2_LIBRARY_DIRS} ${SDL2MIXER_LIBRARY_DIRS} ${PHYSFS_LIBRARY_DIRS})
    set(SAIS_INCLUDES ${SDL2_INCLUDE_DIRS} ${SDL2MIXER_INCLUDE_DIRS} ${PHYSFS_INCLUDE_DIRS})
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
    add_executable(strangedemo WIN32 ${SAIS_SRC})
    target_link_directories(strangedemo PRIVATE ${SAIS_LIB_DIRS})
    target_link_libraries(strangedemo
        PRIVATE
        ${SAIS_LIBS})
    target_include_directories(strangedemo PRIVATE ${SAIS_INCLUDES})
    target_compile_definitions(strangedemo PRIVATE DEMO_VERSION=1)

    install(TARGETS strangedemo RUNTIME DESTINATION .)
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

if(CMAKE_SYSTEM_NAME MATCHES Darwin)
    set_target_properties(strange PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_BUNDLE_NAME "Strange Adventures"
            MACOSX_BUNDLE_COPYRIGHT "Copyright © 2020 Chris Collins, Copyright © 2005 Richard Carlson, Iikka Keranen and William Sears"
            MACOSX_BUNDLE_ICON_FILE "sais.icns")
    target_sources(strange PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/full-data/saisdata.zip" "src/sais.icns")
    set_property(TARGET strange PROPERTY
            RESOURCE "${CMAKE_CURRENT_BINARY_DIR}/full-data/saisdata.zip;src/sais.icns")
    set_property(SOURCE "${CMAKE_CURRENT_BINARY_DIR}/full-data/saisdata.zip" PROPERTY GENERATED TRUE)
    add_dependencies(strange retail_data)
endif()

if(USE_LOG)
    add_compile_definitions(LOG_OUTPUT=1)
endif()

if(NOT BUILD_DEMO)
    set(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
    set(CPACK_WIX_UPGRADE_GUID E709D29C-1BD8-4679-A8CA-9D40567771F5)
    if(CMAKE_SYSTEM_NAME MATCHES Linux)
        set(CPACK_GENERATOR TGZ)
    elseif(CMAKE_SYSTEM_NAME MATCHES Windows)
        set(CPACK_GENERATOR ZIP;WIX)
    elseif(CMAKE_SYSTEM_NAME MATCHES Darwin)
        set(CPACK_GENERATOR ZIP;DragNDrop)
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
