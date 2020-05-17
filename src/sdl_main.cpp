#include <SDL.h>
#include <SDL_mixer.h>
#include <physfs.h>

#include "Typedefs.h"
#include "gfx.h"

int my_main();
int sound_init();

extern SDL_Window *sdlWind;
extern SDL_Surface *sdlsurf;

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
    if (!PHYSFS_setSaneConfig("FreeSAIS", "SAIS", "zip", 0, 1)) {
        SDL_Log("PhysFS failed to set default env: %s", PHYSFS_getLastError());
        return 1;
    }


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
    int sdlFlags = 0;
    if (gfx_fullscreen) {
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

    // create the i8 surface
    sdlsurf = SDL_CreateRGBSurfaceWithFormat(0, 640, 480, 8, SDL_PIXELFORMAT_INDEX8);
    if (sdlsurf == nullptr) {
        SDL_Log("Failed to create INDEX8 Surface: %s", SDL_GetError());
        return 1;
    }
    my_main();
    return 0;
}
