// ----------------
//     INCLUDES
// ----------------

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
#include <time.h>
#include <SDL.h>

#include "typedefs.h"
#include "is_fileio.h"

#include "textstr.h"
#include "iface_globals.h"
#include "gfx.h"
#include "snd.h"
#include "interface.h"
#include "starmap.h"
#include "combat.h"
#include "cards.h"
#include "startgame.h"
#include "endgame.h"

#define MAIN_INTERFACE_COLOR 0

#ifdef MOVIE
extern int movrecord;
#endif

//		extern FILE* loggy;
extern SDL_Surface *sdlsurf;

// ----------------
// GLOBAL VARIABLES
// ----------------

int opt_verbose = 0;

//int from_splash = 1;

//int still_running = 1;

t_ik_spritepak *spr_titles;

// ----------------
// LOCAL VARIABLES
// ----------------

// ----------------
// LOCAL PROTOTYPES
// ----------------

int32 main_menu();

void main_init();
void main_deinit();
void splash_screen();
void credits_screen();
int32 intro_screen();
int modconfig_main();

// ----------------
// GLOBAL FUNCTIONS
// ----------------

int my_main()
{
	Game_Init();

#ifdef MOVIE
	movrecord = 0;
#endif

	must_quit = 0;

#ifndef DEMO_VERSION
	settings.opt_mousemode = 0;
	ik_hidecursor();
	if (!modconfig_main())
		must_quit = 1;
#endif

#ifdef MOVIE
	movrecord = 1;
#endif

	if (!must_quit)
	{
		main_init();

		splash_screen();

		while (!must_quit && main_menu()>0);

		credits_screen();

		main_deinit();
	}

	Game_Shutdown();

	return 0;
}

void splash_screen()
{
	int32 end;
	int32 c, mc;
	int32 t, s, l, z;
	int32 x, y, co;
	t_ik_image *bg[3];
	int32 zap;

	bg[0] = ik_load_pcx("graphics/cheapass.pcx", NULL);
	bg[1] = ik_load_pcx("graphics/digieel.pcx", NULL);
	bg[2] = ik_load_pcx("graphics/title.pcx", NULL);

	prep_screen();
	ik_drawbox(screen, 0, 0, 640, 480, 0);
	ik_blit();
	update_palette();

	end = 0;
#ifdef MOVIE
	start_ik_timer(2, 60);
#else
	start_ik_timer(2, 20);
#endif
	while (!end && !must_quit)
	{
		ik_eventhandler();
		if (get_ik_timer(2) > 50)
			end = 1;
	}

	Play_SoundFX(WAV_LOGO);

	end = 0;
#ifdef MOVIE
	start_ik_timer(2, 60);
#else
	start_ik_timer(2, 20);
#endif
	while (!end && !must_quit)
	{
		ik_eventhandler();
		c = ik_inkey();
		mc = ik_mclick();

		if (mc==1 || c>0)
		{ must_quit = 1; }

		c = t; t = get_ik_timer(2);
		if (t >= 315)
			end = 1;
		else if (t > c)
		{
			prep_screen();
			s = t / 150;
			c = t % 150;

			if (c < 15)
			{
				l = c;
				for (y = 0; y < 480; y++)
					for (x = 0; x < 640; x++)
					{
						co = 0;
						if (s > 0)
							co = gfx_lightbuffer[bg[s-1]->data[y*bg[s-1]->pitch+x]+((15-l)<<8)];

						if (s < 2)
							co = gfx_addbuffer[gfx_lightbuffer[bg[s]->data[y*bg[s]->pitch+x]+(l<<8)] + (co<<8)];

						ik_putpixel(screen, x, y, co);
					}
			}
			else if (s < 2)
				ik_copybox(bg[s], screen, 0, 0, 640, 480, 0,0);
			else
				ik_drawbox(screen, 0, 0, 640, 480, 0);
			
			ik_blit();
		}
	}

	prep_screen();
	ik_drawbox(screen, 0, 0, 640, 480, 0);
	ik_blit();

	Play_Sound(WAV_MUS_START, 15, 1);

	end = 0;
	while (!end && !must_quit)
	{
		ik_eventhandler();
		if (get_ik_timer(2) > 405)
			end = 1;
	}


	zap = 340;
#ifdef MOVIE
	start_ik_timer(2, 60);
#else
	start_ik_timer(2, 20);
#endif
	end = 0; t = get_ik_timer(2);
	while (!end && !must_quit)
	{
		ik_eventhandler();
		c = ik_inkey();
		mc = ik_mclick();

		if (mc==1 || c>0)
		{ must_quit = 1; }
		c = t; t = get_ik_timer(2);
		if (t > c)
		{
			while (t > c)
			{
				c++;
				if (c == 80)
					Play_SoundFX(WAV_TITLE1);
				if (c == 180)
					Play_SoundFX(WAV_TITLE2);
				if (c == 260)
					Play_SoundFX(WAV_TITLE3);

				if (c == 340)
					Play_Sound(WAV_MUS_SPLASH, 15, 1);

				if (c == zap)
					Play_SoundFX(WAV_TITLE4+(rand()&1), 0, 50);
				if (c == zap + 100)
					zap += 150+rand()%150;
			}


			prep_screen();

			if (t > zap && t < zap + 100)
			{
				s = sin1k[((t-zap)*512)/100]>>8;
//			s = sin1k[(t*5)&1023]>>8;
//			s = (s*s*s) >> 16;
			}
			else
				s = 0;

			if (s < 1)
				s = 0;
			else
				s = (rand()%s)>>5;

			if (t < 30)
			{
				l = t/2;
				for (y = 0; y < 480; y++)
					for (x = 0; x < 640; x++)
					{
						co = gfx_lightbuffer[bg[2]->data[y*bg[2]->pitch+x]+(l<<8)];

						ik_putpixel(screen, x, y, co);
					}
			}
			else
				ik_copybox(bg[2], screen, 0, 0, 640, 480, 0,0);

			if (t > 80)
			{
				if (t-80 < 50)
				{	
					z = 192 + ((t-80)*96)/50;
					l = 13*16 + 5-(t-80)/10; 
					ik_drsprite(screen, 104, 80, 0, z, spr_titles->spr[1], 5+(l<<8));
					l = 13*16 + 15-(t-80)/5; 
				}
				else
				{	l = 13*16+6+s; }

				ik_drsprite(screen, 104, 80, 0, 192, spr_titles->spr[1], 5+(l<<8));
			}
			if (t > 180)
			{
				if (t-180 < 50)
				{	
					z = 192 + ((t-180)*96)/50;
					l = 13*16 + 5-(t-180)/10; 
					ik_drsprite(screen, 104, 224, 0, z, spr_titles->spr[2], 5+(l<<8));
					l = 13*16 + 15-(t-180)/5; 
				}
				else
				{	l = 13*16+6+s; }

				ik_drsprite(screen, 104, 224, 0, 192, spr_titles->spr[2], 5+(l<<8));
			}
			if (t > 260)
			{
				if (t-260 < 50)
				{	
					z = 256 + ((t-260)*128)/50;
					l = 13*16 + 5-(t-260)/10; 
					ik_drsprite(screen, 536, 144, 0, z, spr_titles->spr[3], 5+(l<<8));
					l = 13*16 + 15-(t-260)/5; 
				}
				else
				{	l = 13*16+6+s; }

				ik_drsprite(screen, 536, 144, 0, 256, spr_titles->spr[3], 5+(l<<8));
			}
			if (t > 340)
			{
				if (t-340 < 50)
					l = 12*16 + 15-(t-340)/5;
				else
					l = 12*16+6+s;

				ik_drsprite(screen, 320, 352, 
						0, 
						640+(sin1k[(t*8)&1023]>>12), spr_titles->spr[0], 5+(l<<8));
			}

			if (t > 500)
				ik_print(screen, font_6x8, 320-13*3, 448, 0, "PRESS ANY KEY");

			ik_blit();
		}
	}

	del_image(bg[0]);
	del_image(bg[1]);
	del_image(bg[2]);

	must_quit = 0;

}

void credits_screen()
{
	int32 end;
	int32 c, mc;
	int32 t, s, l, r;
	int32 x, y, co;
	t_ik_image *bg[3];

	must_quit = 0;

	bg[0] = ik_load_pcx("graphics/credits1.pcx", NULL);
	bg[1] = ik_load_pcx("graphics/credits2.pcx", NULL);
	bg[2] = ik_load_pcx("graphics/credits3.pcx", NULL);

	prep_screen();
	ik_drawbox(screen, 0, 0, 640, 480, 0);
	ik_blit();
	update_palette();

	Play_Sound(WAV_MUS_SPLASH, 15, 1);

	end = 0;
#ifdef MOVIE
	start_ik_timer(2, 60);
#else
	start_ik_timer(2, 20);
#endif
	while (!end && !must_quit)
	{
		ik_eventhandler();
		c = ik_inkey();
		mc = ik_mclick();
		if (get_ik_timer(2) > 10)
			end = 1;
	}

	end = 0;
#ifdef MOVIE
	start_ik_timer(2, 60);
#else
	start_ik_timer(2, 20);
#endif
	while (!end && !must_quit)
	{
		ik_eventhandler();
		c = ik_inkey();
		mc = ik_mclick();

		if (mc==1 || c>0)
		{ must_quit = 1; }

		
		c = t; t = get_ik_timer(2);
		if (t >= 1765)
			end = 1;
		else if (t > c)
		{
			while (c < t)
			{
				c++;
				if (c == 1700)
					Play_SoundFX(WAV_TITLE4+(rand()&1));
			}

			prep_screen();
			s = (t>300)+(t>1500)+(t>1750);
			if (!s)
				c = t;
			else if (s == 1)
				c = t - 300;
			else if (s == 2)
				c = t - 1500;
			else if (s == 3)
				c = t - 1750;

			if (c < 15)
			{
				l = c;
				for (y = 0; y < 480; y++)
					for (x = 0; x < 640; x++)
					{
						co = 0;
						if (s > 0)
							co = gfx_lightbuffer[bg[s-1]->data[y*bg[s-1]->pitch+x]+((15-l)<<8)];

						if (s < 3)
							co = gfx_addbuffer[gfx_lightbuffer[bg[s]->data[y*bg[s]->pitch+x]+(l<<8)] + (co<<8)];

						ik_putpixel(screen, x, y, co);
					}
			}
			else if (s < 3)
				ik_copybox(bg[s], screen, 0, 0, 640, 480, 0,0);
			else
				ik_drawbox(screen, 0, 0, 640, 480, 0);

			if (t > 1700)
			{
				r = rand()%(1+(((t-1700)*14)/65));
				r = r << 8;
				for (y = 0; y < 480; y++)
					for (x = 0; x < 640; x++)
					{
						co = screen->data[y*screen->pitch+x];
						ik_putpixel(screen, x, y, gfx_addbuffer[r + co]);
					}
			}
			
			ik_blit();
		}
	}

	prep_screen();
	ik_drawbox(screen, 0, 0, 640, 480, 0);
	ik_blit();

	del_image(bg[0]);
	del_image(bg[1]);
	del_image(bg[2]);

	must_quit = 0;

}

int32 main_menu()
{
	int i;

	i = intro_screen();

	if (i)
	{
		if (i==1)	// start game
		{
			if (startgame())	
			{	
#ifdef LOG_OUTPUT
				ik_start_log();
#endif
				Stop_All_Sounds();
				ik_print_log("launching game...\n");
				starmap();
				if (logfile)
					fclose(logfile);
			}
		}
		else	// combat sim
		{
			combat_sim();
		}
		must_quit = 0;
		return 1;
	}
	Stop_All_Sounds();

	return 0;
}

// ----------------
// LOCAL FUNCTIONS
// ----------------

void main_init()
{
	int x;
	FILE *fil;
	must_quit=0;
	wants_screenshot=0;

	fil = myopen("graphics/palette.dat", "rb");
	fread(globalpal, 1, 768, fil);
	fclose(fil);
	memcpy(currentpal, globalpal, 768);

	for (x=0;x<1024;x++)
	{
		sin1k[x] = (int32)(sin(x*3.14159/512)*65536);
		cos1k[x] = (int32)(cos(x*3.14159/512)*65536);
	}

//	if (strlen(moddir))	// loading a mod, check for new frames
//	{
//		loggy = fopen("modlog.txt", "wt");
//	}

	calc_color_tables(globalpal);

	textstrings_init();
	load_all_sfx();
	combat_init();
	starmap_init();
	interface_init();
	cards_init();
	endgame_init();
	gfx_initmagnifier();

	srand( (unsigned)time( NULL ) );

	//s_volume = 85;
	got_hiscore = -2;
	loadconfig();

	spr_titles = load_sprites("graphics/titles.spr");

//	if (strlen(moddir))	// loading a mod, check for new frames
//	{
//		fclose(loggy);
//	}

	if (!(settings.opt_mousemode & 1))
		ik_hidecursor();
	else
		ik_showcursor();

#ifdef MOVIE
	start_ik_timer(2, 60);
#else
	start_ik_timer(2, 20);
#endif

}

void main_deinit()
{
	gfx_deinitmagnifier();
	endgame_deinit();
	cards_deinit();
	interface_deinit();
	starmap_deinit();
	combat_deinit();
	Delete_All_Sounds();
	textstrings_deinit();

	free_spritepak(spr_titles);

	del_color_tables();
}

int32 intro_screen()
{
	int32 end;
	int32 t0, t;
	int32 c, d[4], p, l[4];
	int32 mc, mx, my;
	int32 tx[4], ty[4], tx1[4], ty1[4];
	int32 sx[32], sy[32], sz[32], sc[32], sl;
	uint8 *gp[4];
	int32 x, y;
	int32 fr, fc = 0;
	int32 bx, by, h;
	int32 nebn = 4, starn = 32;
	uint8 *dr, *bk;
	int8 mode = 0;
	//int8 showhiscores = 0;
	int8 hiscmusic = 0;
	int32 fra = 0;

	t_ik_image *nebby;
	t_ik_image *backy;

	if (got_hiscore > -1)
	{
		mode = 1;
		//showhiscores = 1;
	}

	Stop_All_Sounds();

	if (got_hiscore > -1)
	{
		Play_Sound(WAV_MUS_HISCORE, 15, 1);
		hiscmusic = 1;
	}
	else
		Play_Sound(WAV_MUS_THEME, 15, 1);

	backy = ik_load_pcx("graphics/titback.pcx", NULL);
	

	nebby = new_image(640, 480);
	ik_drawbox(nebby, 0, 0, 639, 479, 0);

	start_ik_timer(0, 20); t = 0;

	for (c = 0; c < starn; c++)
	{
		sx[c] = (rand()%512-256)<<9;
		sy[c] = (rand()%512-256)<<9;
		sz[c] = rand()%768 + 128;
		sc[c] = rand()%8;
	}

	end = 0;
	while (!end && !must_quit)
	{
		ik_eventhandler();
		t0 = t; t = get_ik_timer(0);
		c = ik_inkey();
		mc = ik_mclick();
		mx = ik_mouse_x;
		my = ik_mouse_y;

		if (c == 13 || c == 32)
		{	end = 2; Play_SoundFX(WAV_DOT2, 0, 50); }

//		if (c == 'r')
//		{ end = 1; still_running = 1; }

		if (mc & 1)
		{
			if (my > 420 && my < 436)
			{
				if (mx > 176 && mx < 304)	// start game
				{	end = 2; Play_SoundFX(WAV_DOT2, 0, 50); }
				else if (mx > 336 && mx < 464)	// combat sim
				{ end = 3; Play_SoundFX(WAV_DOT2, 0, 50); }
			}
			else if (my > 440 && my < 456)
			{
				if (mx > 96 && mx < 224) // settings
				{
					if (mode == 2)
						mode = 0;
					else
						mode = 2;
					Play_SoundFX(WAV_DOT, 0, 50); 
					if (hiscmusic)
					{
						got_hiscore = -2;
						Play_Sound(WAV_MUS_THEME, 15, 1);
						hiscmusic = 0;
					}
				}
				if (mx > 256 && mx < 384)
				{	
					if (mode == 1)
						mode = 0;
					else
						mode = 1;
					Play_SoundFX(WAV_DOT, 0, 50); 
					if (hiscmusic)
					{
						got_hiscore = -2;
						Play_Sound(WAV_MUS_THEME, 15, 1);
						hiscmusic = 0;
					}
				}
				else if (mx > 416 && mx < 544)
					end = 1;
			}
			else
			{
				if (my > 112 && my < 320 && mx > 16 && mx < 624 && mode == 1)
				{
					Play_SoundFX(WAV_DOT, 0, 50); 
					if (hiscmusic)
					{
						got_hiscore = -2;
						Play_Sound(WAV_MUS_THEME, 15, 1);
						hiscmusic = 0;
					}
					mode = 0;
				}

			}

			if (mode == 2)	// settings
			{
				if (mx > bx+16 && mx < bx+32 && my > by+27 && my < by+27+7*16)
				{
					c = (my - (by+27)) / 16;
					switch (c)
					{
						case 0:
						settings.opt_mucrontext = settings.opt_mucrontext ^ 1;
						break;

						case 1:
						settings.opt_timerwarnings = 1 - settings.opt_timerwarnings;
						break;

						case 2:
						settings.opt_timeremaining = 1 - settings.opt_timeremaining;
						break;

						case 3:
						settings.opt_lensflares = 1 - settings.opt_lensflares;
						break;

						case 4:
						settings.opt_smoketrails = 1 - settings.opt_smoketrails;
						break;

						case 5:
						settings.opt_mousemode = settings.opt_mousemode ^ 1;
						if (settings.opt_mousemode & 1)
							ik_showcursor();
						else
							ik_hidecursor();
						break;

						case 6:
						settings.opt_mucrontext = settings.opt_mucrontext ^ 2;
						break;

						default: ;
					}
				}
				if (mx > bx+32 && mx < bx+160 && my > by+158 && my < by+166)
				{
					settings.opt_volume = ((mx - (bx+26))*10) / 128;
					s_volume = settings.opt_volume * 10;
					Set_Sound_Volume(15, 100);
					Play_SoundFX(WAV_SLIDER, 0, 50);
				}

				if (mx > bx+192 && mx < bx+240 && my > by+h-32 && my < by+h-16)
				{	Play_SoundFX(WAV_DOT, 0, 50);  mode = 0; }
			}
		}

		if (t > t0)
		{
			fr = 0;
			while (t0 < t)
			{
				for (c = 0; c < starn; c++)
				{
					sz[c]-=4;
					if (sz[c] < 128)
					{
						sz[c] = 896;
						sx[c] = (rand()%512-256)<<9;
						sy[c] = (rand()%512-256)<<9;
						sc[c] = rand()%8;
					}
				}
				t0++;
				fr++;
			}

			prep_screen();

			//ik_drawbox(screen, 0, 0, 639, 479, 0);

			//ik_copybox(backy, nebby, 0, 0, 639, 479, 0, 0);
			
			// draw zooming nebula
			fra = (fra + 1) & 3;
			for (c = 0; c < nebn; c++)
			{
				d[c] = 1024-((t*4+(c*768)/nebn)%768);	// distance to nebula plane ( 256...1024)
				tx[c] = 65536-d[c]*160+sin1k[c*300]/2; 
				ty[c] = 65536-d[c]*120+cos1k[c*300]/2;	// corner coords
				if (d[c] < 512)
					l[c] = (d[c]-256)/16;
				else if (d[c] > 768)
					l[c] = (1024-d[c])/16;
				else
					l[c] = 15;

				ty1[c] = ty[c];
			}

			for (y = 0; y < 240; y++)
			{
				dr = ik_image_pointer(nebby, fra&1, y*2+((fra&2)==2));
				bk = ik_image_pointer(backy, fra&1, y*2+((fra&2)==2));

				for (c = 0; c < nebn; c++)
				{
					tx1[c] = tx[c];
					gp[c] = combatbg2->data + ((ty1[c]>>9)&255)*combatbg2->pitch;
				}

				for (x = 0; x < 320; x++)
				{
					p = 0;

					for (c = 0; c < nebn; c++)
					{
						p = gfx_addbuffer[gfx_lightbuffer[gp[c][(tx1[c]>>9)&255]+(l[c]<<8)]+(p<<8)];
						tx1[c] += d[c];
					}

					c = *bk++;
					bk++;

					c = gfx_lightbuffer[(c<<8)+15-MIN(15,(p&15)*2)];
					p = gfx_addbuffer[(c<<8)+p];

					*dr++ = p;
					dr++;
				}

				for (c = 0; c < nebn; c++)
					ty1[c] += d[c];
			}

			ik_copybox(nebby, screen, 0, 0, 640, 480, 0, 0);

			// draw stars			
			for (c = 0; c < starn; c++)
			{
				x = 320 + sx[c] / sz[c];
				y = 240 + sy[c] / sz[c];
				p = 8192 / sz[c];
				if (sz[c] < 192)
					sl = sc[c]*16 + (sz[c]-128)/4;
				else if (sz[c] > 384)
					sl = sc[c]*16 + (896-sz[c])/32;
				else
					sl = sc[c]*16 + 15;
				ik_drsprite(screen, x, y, c<<6, p, spr_shockwave->spr[4], 5+(sl<<8));
			}

			//ik_print(screen, font_6x8, 4, 4, 0, "%d FPS", 50/fr);

			switch (mode)
			{
				case 0:
				//ik_dsprite(screen, 64, 128, spr_title->spr[0], 0);
//				ik_drsprite(screen, 320, 192, 
//						0, 
//						640+(sin1k[(t*8)&1023]>>12), spr_titles->spr[0], 5+((6*16+15)<<8));
				ik_drsprite(screen, 320, 192, 0, 640, spr_titles->spr[0], 5+((6*16+15)<<8));
				break;

				case 1:
				for (y = 112; y < 320; y++)
				{
					dr = ik_image_pointer(screen, 16, y);
					for (x = 16; x < 624; x++)
					{
						*dr++ = gfx_lightbuffer[(*dr)+(8<<8)];
					}
				}
				ik_print(screen, font_6x8, 32, 128, 11, "TOP 20 EXPLORERS");
				ik_print(screen, font_6x8, 578, 128, 11, "SCORE");
				for (c = 0; c < num_scores; c++)
				{
					// 50 +  + 12
					ik_print(screen, font_6x8, 32, 144+c*8, 11-8*(c==got_hiscore), textstring[STR_HISCORE_ENTRY], 
									scores[c].cname, scores[c].sname, scores[c].deathmsg);
					ik_print(screen, font_6x8,572, 144+c*8, 11-8*(c==got_hiscore), "%6d", scores[c].score);
				}
				ik_drawline(screen, 32, 138, 608, 138, 176+12, 0, 255, 0);
				ik_drawline(screen, 32, 139, 608, 139, 176+2, 0, 255, 0);
				break;

				case 2:
				bx = 192; by = 136; h = 184;
				interface_drawborder(screen, bx, by, bx+256, by+h, 1, MAIN_INTERFACE_COLOR, "game settings");

				y = 32;
				ik_print(screen, font_6x8, bx+32, by+y, MAIN_INTERFACE_COLOR, "DISPLAY MISSION BRIEFING");
				ik_dsprite(screen, bx+16, by+y-5, spr_IFbutton->spr[(settings.opt_mucrontext & 1)], 2+(MAIN_INTERFACE_COLOR<<8));

				y+=16;
				ik_print(screen, font_6x8, bx+32, by+y, MAIN_INTERFACE_COLOR, "ENABLE TIME LEFT WARNINGS");
				ik_dsprite(screen, bx+16, by+y-5, spr_IFbutton->spr[settings.opt_timerwarnings], 2+(MAIN_INTERFACE_COLOR<<8));

				y+=16;
				ik_print(screen, font_6x8, bx+32, by+y, MAIN_INTERFACE_COLOR, "DISPLAY TIME LEFT AS COUNTDOWN");
				ik_dsprite(screen, bx+16, by+y-5, spr_IFbutton->spr[settings.opt_timeremaining], 2+(MAIN_INTERFACE_COLOR<<8));

				y+=16;
				ik_print(screen, font_6x8, bx+32, by+y, MAIN_INTERFACE_COLOR, "ENABLE EXPLOSION HIGHLIGHTS");
				ik_dsprite(screen, bx+16, by+y-5, spr_IFbutton->spr[settings.opt_lensflares], 2+(MAIN_INTERFACE_COLOR<<8));

				y+=16;
				ik_print(screen, font_6x8, bx+32, by+y, MAIN_INTERFACE_COLOR, "ENABLE MISSILE SMOKE TRAILS");
				ik_dsprite(screen, bx+16, by+y-5, spr_IFbutton->spr[settings.opt_smoketrails], 2+(MAIN_INTERFACE_COLOR<<8));

				y+=16;
				ik_print(screen, font_6x8, bx+32, by+y, MAIN_INTERFACE_COLOR, "ENABLE HARDWARE MOUSE CURSOR");
				ik_dsprite(screen, bx+16, by+y-5, spr_IFbutton->spr[settings.opt_mousemode & 1], 2+(MAIN_INTERFACE_COLOR<<8));

				y+=16;
				ik_print(screen, font_6x8, bx+32, by+y, MAIN_INTERFACE_COLOR, "LARGE TRADING SCREEN");
				ik_dsprite(screen, bx+16, by+y-5, spr_IFbutton->spr[(settings.opt_mucrontext & 2)/2], 2+(MAIN_INTERFACE_COLOR<<8));

				y+=16;
				ik_print(screen, font_6x8, bx+32, by+y, MAIN_INTERFACE_COLOR, "SOUND VOLUME: %d%%", s_volume);
				interface_drawslider(screen, bx+32, by+y+14, 0, 128, 10, settings.opt_volume, MAIN_INTERFACE_COLOR);

				interface_drawbutton(screen, bx+256-64, by+h-32, 48, MAIN_INTERFACE_COLOR, "OK");

				//ik_print(screen, font_6x8, 224, 192, MAIN_INTERFACE_COLOR, "SHOW MISSION BRIEFING");
				break;
				default: ;
			}

			interface_drawbutton(screen, 176,  420, 128, MAIN_INTERFACE_COLOR, "START GAME");
			interface_drawbutton(screen, 336, 420, 128, MAIN_INTERFACE_COLOR, "COMBAT SIMULATOR");

			interface_drawbutton(screen, 96,  440, 128, MAIN_INTERFACE_COLOR, "SETTINGS");
			interface_drawbutton(screen, 256, 440, 128, MAIN_INTERFACE_COLOR, "HIGH SCORES");
			interface_drawbutton(screen, 416, 440, 128, MAIN_INTERFACE_COLOR, "QUIT");

			ik_print(screen, font_6x8, 3, 3, 0, SAIS_VERSION_NUMBER);

			ik_blit();	
		}
	}

	got_hiscore = -2;
	saveconfig();

	del_image(nebby);
	del_image(backy);

	if (end)
		return end-1;
	else
		return 0;
}