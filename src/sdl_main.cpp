#include <SDL.h>
#include <SDL_mixer.h>


#include "typedefs.h"
#include "gfx.h"

int my_main();
int sound_init();

extern SDL_Surface *sdlsurf;

int main(int argc, char *argv[])
{
	gfx_width=640; gfx_height=480; 
	gfx_fullscreen=0; 

	c_minx=0; 
	c_miny=0;
	c_maxx=gfx_width;
	c_maxy=gfx_height;

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
	{	
		//sdlerr("initialising SDL"); 
		return 1; 
	}
	SDL_WM_SetCaption("Strange Adventures In Infinite Space", "Strange Adventures In Infinite Space");

	// Enable UNICODE so we can emulate getch() in text input
	SDL_EnableUNICODE(1);

	// init SDL mixer
	if (Mix_OpenAudio(22050, AUDIO_S16, 2, 1024) < 0)
	{
		return 1;
	}
	Mix_AllocateChannels(16);
	sound_init();

	if (gfx_fullscreen)
		sdlsurf = SDL_SetVideoMode(640, 480, 8, SDL_FULLSCREEN | SDL_HWPALETTE);
	else
		sdlsurf = SDL_SetVideoMode(640, 480, 8, SDL_HWPALETTE);
	my_main();

	return 0;
}
