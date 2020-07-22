/* Strange Adventures in Infinite Space
 * Copyright (C) 2005 Richard Carlson, Iikka Keranen and William Sears
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include <SDL.h>
#include <SDL_mixer.h>
#include <physfs.h>
#include <string>

#include "Typedefs.h"
#include "gfx.h"
#include "iface_globals.h"
#include "is_fileio.h"
#include "port.h"

int my_main();

int sound_init();

extern SDL_Window *sdlWind;
extern SDL_Surface *sdlsurf;
SDL_Surface *blitIntermedSurf = nullptr;
SDL_Renderer *sdlRend = nullptr;
SDL_Texture *sdlWindTexture = nullptr;
// directory paths for our core.

std::string fsBaseDir;
std::string fsPreferencesDir;

static int8 isPixelPerfect = -1;
static int8 isFullscreen = -1;

int
vid_reset_settings() {
    if (sdlWind != nullptr) {
        if (isFullscreen != globalsettings.opt_fullscreen) {
            isFullscreen = globalsettings.opt_fullscreen;
            if (isFullscreen) {
                SDL_SetWindowFullscreen(sdlWind, SDL_WINDOW_FULLSCREEN_DESKTOP);
            } else {
                SDL_SetWindowFullscreen(sdlWind, SDL_WINDOW_RESIZABLE);
                SDL_SetWindowSize(sdlWind, 640, 480);
                SDL_SetWindowMinimumSize(sdlWind, 640, 480);
                sdl_screen_scale = 1.0;
                sdl_x_offset = 0;
                sdl_y_offset = 0;
            }
        }
        if (sdlRend != nullptr) {
            if (globalsettings.opt_whole_multiple_rescale_ratio != isPixelPerfect
                || sdlWindTexture == nullptr) {
                isPixelPerfect = globalsettings.opt_whole_multiple_rescale_ratio;
                if (isPixelPerfect) {
                    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
                } else {
                    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
                }
                if (sdlWindTexture != nullptr) {
                    SDL_DestroyTexture(sdlWindTexture);
                    sdlWindTexture = nullptr;
                }
                // and create the streaming texture.
                sdlWindTexture = SDL_CreateTexture(sdlRend, blitIntermedSurf->format->format,
                                                   SDL_TEXTUREACCESS_STREAMING, 640, 480);
                if (sdlWindTexture == nullptr) {
                    SYS_abort("Failed to create intermediate window texture (for fullscreen scaling): %s", SDL_GetError());
                }
            }
        }
    }
    return 0;
}

void
load_globalsettings() {
    memset(&globalsettings, 0, sizeof(globalsettings));
    auto gsFile = IS_Open_Read("globalsettings.dat");
    if (nullptr == gsFile) {
        memset(&globalsettings, 0, sizeof(globalsettings));
        return;
    }
    IS_Read(&globalsettings, 1, sizeof(globalsettings), gsFile);
    IS_Close(gsFile);

    vid_reset_settings();
}

void
save_globalsettings() {
    auto gsFile = IS_Open_Write("globalsettings.dat");
    if (nullptr == gsFile) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't open globalsettings.dat for write: %s",
                     PHYSFS_getLastError());
        return;
    }
    IS_Write(&globalsettings, sizeof(globalsettings), 1, gsFile);
    IS_Close(gsFile);
}


int main(int argc, char *argv[]) {
    gfx_width = 640;
    gfx_height = 480;
    gfx_fullscreen = 0;

    c_minx = 0;
    c_miny = 0;
    c_maxx = gfx_width;
    c_maxy = gfx_height;

    if (!PHYSFS_init(argv[0])) {
        SYS_abort("PhysFS failed to initialise: %s", PHYSFS_getLastError());
    }

#if TARGET_OS_OSX
    /* Under macOS, we need to use a "non-standard" mount heirachy.
     *
     * We ONLY mount the pref dir and the game data zipfile here - we don't
     * use the BaseDir (which points to the top level of the resource bundle)
    */
    const char *prefDirCStr = PHYSFS_getPrefDir("FreeSAIS", "SAIS");

    if (!PHYSFS_mount(prefDirCStr, "/", 0)) {
        SYS_abort("Failed to mount write directory \"%s\": %s", prefDirCStr, PHYSFS_getLastError());
    }
    if (!PHYSFS_setWriteDir(prefDirCStr)) {
        SYS_abort("Failed to set write directory to \"%s\": %s", prefDirCStr, PHYSFS_getLastError());
    }
    std::string resourcePath(PHYSFS_getBaseDir());
#ifdef DEMO_VERSION
    resourcePath += "Contents/Resources/saisdemo.zip";
#else
    resourcePath += "Contents/Resources/saisdata.zip";
#endif
    if (!PHYSFS_mount(resourcePath.c_str(), "/", 1)) {
        SYS_abort("Failed to mount data archive \"%s\": %s", resourcePath.c_str(), PHYSFS_getLastError());
    }
#else
    if (!PHYSFS_setSaneConfig("FreeSAIS", "SAIS", "zip", 0, 0)) {
        SYS_abort("PhysFS failed to set default env: %s", PHYSFS_getLastError());
    }
#endif

    load_globalsettings();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
        SYS_abort("Failed to initialise SDL: %s", SDL_GetError());
    }

    // init SDL mixer
    if (Mix_OpenAudio(22050, AUDIO_S16, 2, 1024) < 0) {
        SYS_abort("Failed to open audio device: %s", Mix_GetError());
    }
    Mix_AllocateChannels(16);
    sound_init();

    // create the application window
    int sdlFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL;
    if (globalsettings.opt_fullscreen) {
        sdlFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    sdlWind = SDL_CreateWindow("Strange Adventures In Infinite Space",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               640, 480,
                               sdlFlags);
    if (sdlWind == nullptr) {
        SYS_abort("Failed to create SDL Window: %s", SDL_GetError());
    }
    SDL_SetWindowMinimumSize(sdlWind, 640, 480);


    // create the SDL Renderer we use to stream from the intermed blitting surface to the window.
    sdlRend = SDL_CreateRenderer(sdlWind, -1, SDL_RENDERER_ACCELERATED);
    if (sdlRend == nullptr) {
        SYS_abort("Failed to create SDL Renderer: %s", SDL_GetError());
    }

    // create the i8 surface
    sdlsurf = SDL_CreateRGBSurfaceWithFormat(0, 640, 480, 8, SDL_PIXELFORMAT_INDEX8);
    if (sdlsurf == nullptr) {
        SYS_abort("Failed to create INDEX8 Surface: %s", SDL_GetError());
    }

    // create the intermediate blitting surface (needed for fullscreen support)
    blitIntermedSurf = SDL_CreateRGBSurfaceWithFormat(0, 640, 480, 32, SDL_PIXELFORMAT_RGBA8888);
    if (blitIntermedSurf == nullptr) {
        SYS_abort("Failed to create intermediate target surface: %s", SDL_GetError());
    }

    // make sure our video state matches what we think it should be.  This also
    // creates the SDL_Texture we use to do the screen copy.
    vid_reset_settings();

    my_main();
    return 0;
}
