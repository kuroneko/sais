

#define INITGUID

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <objbase.h>
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
#include <SDL_mixer.h>

#include <dsound.h>

#include "typedefs.h"
#include "iface_globals.h"
#include "snd.h"
#include "gfx.h"




// GLOBALS ////////////////////////////////////////////////

t_sfxchannel sfxchan[NUM_SFX];
t_wavesound wavesnd[WAV_MAX];
// channels are ones actually being played
// samples are sitting in the memory and cloned into channels when needed


int sound_init()
{

	static int first_time = 1; // used to track the first time the function is entered

	// initialize the sound fx array
	for (int index=0; index<WAV_MAX; index++)
	{
		wavesnd[index].name[0] = 0;
		wavesnd[index].wave = NULL;
	} 

	// return sucess
	return(1);
}

Mix_Chunk *lsnd(int32 name)
{
	Mix_Chunk *wave;

	wave = (Mix_Chunk*)wavesnd[name].wave;
	if (!wave)
	{
		wavesnd[name].wave = Mix_LoadWAV(wavesnd[name].name);
		wave = (Mix_Chunk*)wavesnd[name].wave;
	}

	return wave;
}

int Load_WAV(char *filename, int id)
{
	sprintf(wavesnd[id].name, filename);
	lsnd(id);
	return id;
}

///////////////////////////////////////////////////////////

int Play_Sound(int id, int ch, int flags, int volume, int rate, int pan)
{
	// this function plays a sound thru a channel, set flags to make it loop..
	if (flags)
		flags=9999;

	Stop_Sound(ch);
	Mix_PlayChannel(ch, lsnd(id), flags);

	if (volume>=0) Set_Sound_Volume(ch, volume);
		else Set_Sound_Volume(ch, 100);
//	if (rate>=0) Set_Sound_Freq(ch, rate);
//		else Set_Sound_Freq(ch, sound_samples[id].rate);
	if (pan) Set_Sound_Pan(ch, pan);
		else Set_Sound_Pan(ch, 0);

	// return success
	return(1);
}


int Play_SoundFX(int id, int t, int volume, int rate, int pan, int cutoff)
{
	int x;
	int ch,tt,ch0;
	int l;
	Mix_Chunk *chunk;

	t = get_ik_timer(2);

	ch=-1;tt=cutoff;ch0=-1;
	for (x=0;x<NUM_SFX;x++)
	{
		if (sfxchan[x].id==id && sfxchan[x].st<t-10)
		{	ch=x; break; }
		if (sfxchan[x].id==-1)
		{ ch=x; break; }
		if (t>sfxchan[x].et)
		{	ch=x; break; }
	}
	if (ch==-1)
	for (x=0;x<NUM_SFX;x++)
	{
		if (sfxchan[x].et-t<tt)
		{
			ch0=x; tt=sfxchan[x].et-t;
			break;
		}
	}
	if (ch==-1 && ch0>-1)
	{
		ch=ch0;
	}

	if (ch>-1)
	{
		chunk = lsnd(id);
		if (chunk)
		{
			l = chunk->alen;
			sfxchan[ch].id=id;
			sfxchan[ch].st=t;
			if (rate==-1) rate=Get_Sound_Rate(id);
			sfxchan[ch].et=t+(l*50)/22050;
			Play_Sound(id, ch+CHN_SFX, 0, volume, rate, pan);
		}
	}
	
//	Play_Sound(id, CHN_SFX, 0, volume, rate, pan);

	return 1;
}


///////////////////////////////////////////////////////////

int Set_Sound_Volume(int ch,int vol)
{
	// this function sets the volume on a sound 0-100
	vol = (vol * s_volume * 128) / 10000;
	
	Mix_Volume(ch, vol);

	// return success
	return(1);
}

///////////////////////////////////////////////////////////

int Set_Sound_Freq(int ch,int freq)
{
	// this function sets the playback rate
	// ... not really

	return(1);
}

///////////////////////////////////////////////////////////

int Set_Sound_Pan(int ch,int pan)
{
	int lf, rt;
	// this function sets the pan, -10,000 to 10,000

	if (pan < 0)
	{
		lf = 255; rt = (int)(255 - sqrt(-pan)*2);
	}
	else
	{
		rt = 255; lf = (int)(255 - sqrt(pan)*2);
	}

	Mix_SetPanning(ch, lf, rt);

	return(1);
}

////////////////////////////////////////////////////////////

int Stop_All_Sounds(void)
{
	for (int index=0; index<16; index++)
		Stop_Sound(index);	
	for (int x=0; x<NUM_SFX; x++)
	{ sfxchan[x].et=0; sfxchan[x].st=0; sfxchan[x].id=-1; }

	return(1);
}

///////////////////////////////////////////////////////////

int Stop_Sound(int ch)
{
	Mix_HaltChannel(ch);

	return(1);
} 

///////////////////////////////////////////////////////////

int Delete_All_Sounds(void)
{
	for (int index=0; index < WAV_MAX; index++)
		Delete_Sound(index);

	return(1);
} 

///////////////////////////////////////////////////////////

int Delete_Sound(int id)
{
	if (wavesnd[id].wave)
	{
		Mix_FreeChunk((Mix_Chunk*)wavesnd[id].wave);
		wavesnd[id].wave = NULL;
		return(1);
  } 
	
	return(1);
} 

///////////////////////////////////////////////////////////

int Status_Sound(int ch)
{
	if (Mix_Playing(ch))
		return 1;
	return 0;
} 

int Get_Sound_Size(int id)
{
	Mix_Chunk *chunk;
	if (wavesnd[id].wave)
	{
		chunk = (Mix_Chunk*)wavesnd[id].wave;
		return chunk->alen;
	}

	return 0;
}

int Get_Sound_Rate(int id)
{
	return 22050;
}