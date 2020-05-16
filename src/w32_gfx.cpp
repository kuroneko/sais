/*
All DIRECTDRAW stuff here

- DDraw init/shutdown

*/

#define WIN32_LEAN_AND_MEAN  

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream.h> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <SDL.h>

//#include <ddraw.h>  // directX includes

#include "typedefs.h"
#include "iface_globals.h"
#include "gfx.h"

// DEFINES

#define SCREEN_BPP    8                // bits per pixel

// GLOBALS

extern SDL_Surface *sdlsurf;
UCHAR *drawbuffer;  // video buffer
int drawpitch;      // line pitch

#ifdef MOVIE
int when = 0;
int movrecord = 1;
#endif

// blit screen
void ik_blit()
{
	t_ik_sprite *cs;

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

	SDL_UpdateRect(sdlsurf, 0, 0, 640, 480);
	SDL_Flip(sdlsurf);

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
	SDL_SetColors(sdlsurf, spal, 0, 256);
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