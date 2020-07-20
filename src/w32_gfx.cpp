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
/*
All DIRECTDRAW stuff here

- DDraw init/shutdown

*/

#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cmath>

#include <SDL.h>
#include <algorithm>

#include "Typedefs.h"
#include "iface_globals.h"
#include "gfx.h"

// DEFINES

#define SCREEN_BPP    8                // bits per pixel

// GLOBALS
extern SDL_Window *sdlWind;
extern SDL_Surface *sdlsurf;
extern SDL_Surface *blitIntermedSurf;
extern SDL_Renderer *sdlRend;
extern SDL_Texture *sdlWindTexture;

#ifdef MOVIE
int when = 0;
int movrecord = 1;
#endif

int sdl_x_offset = 0;
int sdl_y_offset = 0;
float sdl_screen_scale = 1.0f;

void gfx_refresh_screen() {
    int w,h;
    SDL_GetWindowSize(sdlWind, &w, &h);

    // calculate scale factor.
    if (globalsettings.opt_whole_multiple_rescale_ratio) {
        // for pixel perfect scaling, use integer ratios only.
        int yrat = h / sdlsurf->h;
        int xrat = w / sdlsurf->w;
        sdl_screen_scale = static_cast<float>(std::min<int>(xrat, yrat));
        if (sdl_screen_scale < 1) {
            sdl_screen_scale = 1;
        }
    } else {
        float yrat = static_cast<float>(h) / static_cast<float>(sdlsurf->h);
        float xrat = static_cast<float>(w) / static_cast<float>(sdlsurf->w);
        sdl_screen_scale = std::min<float>(xrat, yrat);
    }

    int targetXSize = static_cast<int>(sdlsurf->w * sdl_screen_scale);
    int targetYSize = static_cast<int>(sdlsurf->h * sdl_screen_scale);
    sdl_x_offset = (w - targetXSize) / 2;
    sdl_y_offset = (h - targetYSize) / 2;

    SDL_Rect destRect{
            sdl_x_offset,
            sdl_y_offset,
            targetXSize,
            targetYSize,
    };
    // clear the surface first.
    SDL_RenderClear(sdlRend);

    // now, blit the 8I surface to the RGBA32 surface
    if (SDL_BlitSurface(sdlsurf, nullptr, blitIntermedSurf, nullptr)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Blit to correct format failed: %s", SDL_GetError());
        abort();
    }

    // now, copy the surface data to the texture.
    char *TexPixelData = nullptr; int pitch = 0;
    if (!SDL_LockTexture(sdlWindTexture, nullptr, reinterpret_cast<void**>(&TexPixelData), &pitch)) {
        SDL_LockSurface(blitIntermedSurf);
        const int lineLength = blitIntermedSurf->format->BytesPerPixel * blitIntermedSurf->w;
        for (int y = 0; y < blitIntermedSurf->h; y++) {
            const int lOffset = pitch * y;
            const int sOffset = blitIntermedSurf->pitch * y;
            memcpy(TexPixelData + lOffset, reinterpret_cast<char *>(blitIntermedSurf->pixels) + sOffset, lineLength);
        }
        SDL_UnlockSurface(blitIntermedSurf);
        SDL_UnlockTexture(sdlWindTexture);
    }

    // and blat the texture onto the screen.
    SDL_RenderCopy(sdlRend, sdlWindTexture, nullptr, &destRect);
    SDL_RenderPresent(sdlRend);
}

// blit screen
void ik_blit() {
    t_ik_sprite *cs = nullptr;

    // take screenshots here (!)
#ifdef MOVIE
    if (get_ik_timer(2) > when && movrecord == 1)
    {
        when += 2;
        wants_screenshot = 1;
    }
#endif
    if (wants_screenshot) {
        ik_save_screenshot(screen, currentpal);
        wants_screenshot = 0;
    }

    if ((settings.opt_mousemode & 5) == 0) {
        cs = get_sprite(screen, ik_mouse_x, ik_mouse_y, 16, 16);
        ik_draw_mousecursor();
    } else if (settings.opt_mousemode & 4) {
//		cs = get_sprite(screen, ik_mouse_x-128, ik_mouse_y-128, 256, 256);
        cs = get_sprite(screen, ik_mouse_x - 192, ik_mouse_y - 96, 384, 192);
        gfx_magnify();
        if (!(settings.opt_mousemode & 1)) {
            ik_draw_mousecursor();
        }
    }
#ifdef DEMO_VERSION
    gfx_blarg();
#endif
    free_screen();

    gfx_refresh_screen();

    if ((settings.opt_mousemode & 5) == 0) {
        prep_screen();
        ik_dsprite(screen, ik_mouse_x, ik_mouse_y, cs, 4);
        free_screen();
        free_sprite(cs);
    } else if (settings.opt_mousemode & 4) {
        prep_screen();
//		ik_dsprite(screen, ik_mouse_x-128, ik_mouse_y-128, cs, 4);
        ik_dsprite(screen, ik_mouse_x - 192, ik_mouse_y - 96, cs, 4);
        free_screen();
        free_sprite(cs);
    }
}

// palette stuff
void update_palette() {
    SDL_Color spal[256];
    int i;

    for (i = 0; i < 256; i++) {
        spal[i].r = currentpal[i * 3];
        spal[i].g = currentpal[i * 3 + 1];
        spal[i].b = currentpal[i * 3 + 2];
    }

    SDL_SetPaletteColors(sdlsurf->format->palette, spal, 0, 256);
}

void set_palette_entry(int n, int r, int g, int b) {
    currentpal[n * 3] = r;
    currentpal[n * 3 + 1] = g;
    currentpal[n * 3 + 2] = b;
}

int get_palette_entry(int n) {
    return currentpal[n * 3] * 65536 +
           currentpal[n * 3 + 1] * 256 +
           currentpal[n * 3 + 2];
}

void prep_screen() // call before drawing stuff to *screen
{
    SDL_LockSurface(sdlsurf);

    screenbuf.data = (uint8 *) sdlsurf->pixels;
    screenbuf.w = sdlsurf->w;
    screenbuf.h = sdlsurf->h;
    screenbuf.pitch = sdlsurf->pitch;
    screen = &screenbuf;
}

void free_screen() // call after drawing, before blit
{
    SDL_UnlockSurface(sdlsurf);
}

int gfx_checkswitch() {
    /*
    // implement windowed/fullscreen switch here?
    */
    return 0;
}
