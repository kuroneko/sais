/*
code to handle all/most of the interaction with the win32 system

- event handling
- kb and mouse input
*/

// INCLUDES ///////////////////////////////////////////////
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

#include "typedefs.h"
#include "iface_globals.h"
#include "gfx.h"
#include "snd.h"

// DEFINES ////////////////////////////////////////////////

// MACROS /////////////////////////////////////////////////

// these read the keyboard asynchronously
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// TYPES //////////////////////////////////////////////////

typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned char  UCHAR;
typedef unsigned char  BYTE;

typedef struct {
	int32 start, freq; 
} t_ik_timer;

// PROTOTYPES /////////////////////////////////////////////

#ifdef MOVIE
extern int movrecord;
#endif

// GLOBALS ////////////////////////////////////////////////

SDL_Surface *sdlsurf;
extern t_paletteentry pe[256];

char buffer[80];                // used to print text
int IsMinimized = 0;
int ActiveApp = 0;
int SwitchMode = 0;

int ik_mouse_x;
int ik_mouse_y;
int ik_mouse_b;
int ik_mouse_c;
int must_quit;
int wants_screenshot;

int key_left=SDLK_LEFT;
int key_right=SDLK_RIGHT;
int key_up=SDLK_UP;
int key_down=SDLK_DOWN;
int key_f[10];
int key_fire1=SDLK_TAB;
int key_fire2=SDLK_RETURN;
int key_fire2b=SDLK_SPACE;

char ik_inchar;
uint8 *keystate;

t_ik_timer ik_timer[10];

// FUNCTIONS //////////////////////////////////////////////

void eventhandler()
{
	SDL_Event event;
	int b;

	keystate = SDL_GetKeyState(NULL);

	while ( SDL_PollEvent(&event) ) 
	{
		switch (event.type) 
		{
			case SDL_KEYDOWN:
			switch(event.key.keysym.sym){
				case SDLK_F12:
					wants_screenshot=1;
					break;
				case SDLK_F2:
				case SDLK_RCTRL:
				case SDLK_LCTRL:
					settings.opt_mousemode ^= 4;
					Play_SoundFX(WAV_LOCK,0);
					break;
				case SDLK_ESCAPE :
					must_quit=1;
					break;
			}

			ik_inchar = event.key.keysym.unicode & 0xff;
			break;

			case SDL_MOUSEBUTTONDOWN:
				b = (event.button.button == SDL_BUTTON_LEFT) +
						2*(event.button.button == SDL_BUTTON_RIGHT) +
						4*(event.button.button == SDL_BUTTON_MIDDLE);
				ik_mouse_c = b;	
				ik_mouse_b |= b;
			case SDL_MOUSEMOTION:
				ik_mouse_x = event.motion.x;
				ik_mouse_y = event.motion.y;
				break;

			case SDL_MOUSEBUTTONUP:
				b = (event.button.button == SDL_BUTTON_LEFT) +
						2*(event.button.button == SDL_BUTTON_RIGHT) +
						4*(event.button.button == SDL_BUTTON_MIDDLE);
				ik_mouse_b &= (7-b);
				break;

			case SDL_ACTIVEEVENT:
			ActiveApp = event.active.gain;
			if (ActiveApp)
			{
				gfx_redraw = 1;
			}
			break;

			case SDL_VIDEOEXPOSE:
			case SDL_VIDEORESIZE:
				ActiveApp = 1;
				break;
			case SDL_QUIT:
				must_quit = 1;
				break;
			default:
				break;
		}
	}
}


// WINX GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////

int Game_Init(void *parms)
{
	int x;

	for (x=0;x<10;x++)
		key_f[x]=SDLK_F1+x;

	return(1);
}

int Game_Shutdown(void *parms)
{
	return(1);
}


///////////////////////////////////////////////////////////

// call eventhandler once every frame
// to check if windows is trying to kill you (or other events)
int ik_eventhandler()
{
	eventhandler();

	if (must_quit)
		return 1;

	return 0;
}

// read key
int key_pressed(int vk_code)
{
	if (keystate)
		return keystate[vk_code];
	else
		return 0;
}

int ik_inkey()
{
	char c=ik_inchar;

	ik_inchar=0;
	return c;
}

int ik_mclick()
{
	char c=ik_mouse_c&3;

	ik_mouse_c=0;
	return c;
}

// cheesy timer functions
void start_ik_timer(int n, int f)
{
	ik_timer[n].start=SDL_GetTicks();
	ik_timer[n].freq=f;
}

void set_ik_timer(int n, int v)
{
	int x;

	x=SDL_GetTicks();
	ik_timer[n].start=x-ik_timer[n].freq*v;
}

int get_ik_timer(int n)
{
	int x;

	if (ik_timer[n].freq)
	{
		x=SDL_GetTicks();
		return ((x-ik_timer[n].start)/ik_timer[n].freq);
	}

	return 0;
}

int get_ik_timer_fr(int n)
{
	int x;

	if (ik_timer[n].freq)
	{
		x=SDL_GetTicks();
		return ((x-ik_timer[n].start)*256/ik_timer[n].freq);
	}

	return 0;
}

void ik_showcursor()
{
	SDL_ShowCursor(1);
}

void ik_hidecursor()
{
	SDL_ShowCursor(0);
}
