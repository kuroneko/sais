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

int my_main();

int sound_init();

extern SDL_Window *sdlWind;
extern SDL_Surface *sdlsurf;
SDL_Surface *blitIntermedSurf = nullptr;

// directory paths for our core.

std::string fsBaseDir;
std::string fsPreferencesDir;

int
vid_reset_settings() {
    if (sdlWind != nullptr) {
        if (globalsettings.opt_fullscreen) {
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
        SDL_Log("PhysFS failed to initialise: %s", PHYSFS_getLastError());
        return 1;
    }
    if (!PHYSFS_setSaneConfig("FreeSAIS", "SAIS", "zip", 0, 0)) {
        SDL_Log("PhysFS failed to set default env: %s", PHYSFS_getLastError());
        return 1;
    }

    load_globalsettings();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
        SDL_Log("Failed to initialise SDL: %s", SDL_GetError());
        return 1;
    }

    // init SDL mixer
    if (Mix_OpenAudio(22050, AUDIO_S16, 2, 1024) < 0) {
        return 1;
    }
    Mix_AllocateChannels(16);
    sound_init();

    // create the application window
    int sdlFlags = SDL_WINDOW_RESIZABLE;
    if (globalsettings.opt_fullscreen) {
        sdlFlags |= SDL_WINDOW_FULLSCREEN;
    }

    sdlWind = SDL_CreateWindow("Strange Adventures In Infinite Space",
                               SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED,
                               640, 480,
                               sdlFlags);
    if (sdlWind == nullptr) {
        SDL_Log("Failed to create SDL Window: %s", SDL_GetError());
        return 1;
    }
    SDL_SetWindowMinimumSize(sdlWind, 640, 480);

    // create the i8 surface
    sdlsurf = SDL_CreateRGBSurfaceWithFormat(0, 640, 480, 8, SDL_PIXELFORMAT_INDEX8);
    if (sdlsurf == nullptr) {
        SDL_Log("Failed to create INDEX8 Surface: %s", SDL_GetError());
        return 1;
    }

    // create the intermediate blitting surface (needed for fullscreen support)
    auto *realSurf = SDL_GetWindowSurface(sdlWind);
    blitIntermedSurf = SDL_ConvertSurface(sdlsurf, realSurf->format, 0);

    my_main();
    return 0;
}
