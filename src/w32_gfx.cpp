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

#include "Typedefs.h"
#include "iface_globals.h"
#include "gfx.h"

// DEFINES

#define SCREEN_BPP    8                // bits per pixel

// GLOBALS
extern SDL_Window  *sdlWind;
extern SDL_Surface *sdlsurf;

#ifdef MOVIE
int when = 0;
int movrecord = 1;
#endif

// blit screen
void ik_blit()
{
	t_ik_sprite *cs = nullptr;

	// take screenshots here (!)
#ifdef MOVIE
	if (get_ik_timer(2) > when && movrecord == 1)
	{
		when += 2;
		wants_screenshot = 1;
	}
#endif
	if (wants_screenshot)
	{
		ik_save_screenshot(screen, currentpal);
		wants_screenshot=0;
	}

	if ((settings.opt_mousemode&5)==0)
	{
		cs = get_sprite(screen, ik_mouse_x, ik_mouse_y, 16, 16);
		ik_draw_mousecursor();
	}
	else if (settings.opt_mousemode & 4)
	{
//		cs = get_sprite(screen, ik_mouse_x-128, ik_mouse_y-128, 256, 256);
		cs = get_sprite(screen, ik_mouse_x-192, ik_mouse_y-96, 384, 192);
		gfx_magnify();
		if (!(settings.opt_mousemode & 1))
		{
			ik_draw_mousecursor();
		}
	}
#ifdef DEMO_VERSION
		gfx_blarg();
#endif
	free_screen();

	auto *realsurf = SDL_GetWindowSurface(sdlWind);
    if (SDL_BlitSurface(sdlsurf, nullptr, realsurf, nullptr)) {
        SDL_Log("Blit Failed?: %s", SDL_GetError());
        abort();
    }
    if (SDL_UpdateWindowSurface(sdlWind)) {
        SDL_Log("Update Surface Failed: %s", SDL_GetError());
        abort();
    }

	if ((settings.opt_mousemode&5)==0)
	{
		prep_screen();
		ik_dsprite(screen, ik_mouse_x, ik_mouse_y, cs, 4);
		free_screen();
		free_sprite(cs);
	}
	else if (settings.opt_mousemode & 4)
	{
		prep_screen();
//		ik_dsprite(screen, ik_mouse_x-128, ik_mouse_y-128, cs, 4);
		ik_dsprite(screen, ik_mouse_x-192, ik_mouse_y-96, cs, 4);
		free_screen();
		free_sprite(cs);
	}
}

// palette stuff
void update_palette()  
{
	SDL_Color spal[256];
	int i;

	for (i = 0; i < 256; i++)
	{
		spal[i].r = currentpal[i*3];
		spal[i].g = currentpal[i*3+1];
		spal[i].b = currentpal[i*3+2];
	}

	SDL_SetPaletteColors(sdlsurf->format->palette, spal, 0, 256);
}

void set_palette_entry(int n, int r, int g, int b)
{
	currentpal[n*3]		= r;
	currentpal[n*3+1]	= g;
	currentpal[n*3+2]	= b;
}

int get_palette_entry(int n)
{
	return currentpal[n*3]*65536 + 
				 currentpal[n*3+1]*256 +
				 currentpal[n*3+2];
}

void prep_screen() // call before drawing stuff to *screen
{
	SDL_LockSurface(sdlsurf);

	screenbuf.data=(uint8*)sdlsurf->pixels;
	screenbuf.w=sdlsurf->w;
	screenbuf.h=sdlsurf->h;
	screenbuf.pitch=sdlsurf->pitch;
	screen=&screenbuf;
}

void free_screen() // call after drawing, before blit
{
	SDL_UnlockSurface(sdlsurf);
}

int gfx_checkswitch()
{
	/*
	// implement windowed/fullscreen switch here?
	*/
	return 0;
}
