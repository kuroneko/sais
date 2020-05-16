// ----------------
//     INCLUDES
// ----------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "typedefs.h"
#include "is_fileio.h"

#include "textstr.h"
#include "iface_globals.h"
#include "gfx.h"
#include "snd.h"
#include "interface.h"
#include "combat.h"
#include "cards.h"
#include "endgame.h"

#include "starmap.h"

#define CS_SHIP 24
#define CS_WING (CS_SHIP+48)
#define CS_FLEET (CS_WING+112)


int32 cs_setupbattle();
void cs_player_init();
void cs_enemy_init(int32 r);
void cs_insertship(int32 st);
void cs_delship(int32 s);

void cs_player_init()
{
	int c;
	int s;

//	strcpy(player.captname, captnames[rand()%num_captnames]);
//	strcpy(player.shipname, shipnames[rand()%num_shipnames]);

	ik_print_log("initializing player...\n");

	memcpy(&shiptypes[0], &shiptypes[1+settings.dif_ship], sizeof(t_shiptype));
	strcpy(shiptypes[0].name, settings.shipname);

	memset(&player, 0, sizeof(t_player));
	strcpy(player.shipname, settings.shipname);
	strcpy(player.captname, settings.captname);

	player.num_ships = 1;
	player.ships[0] = 0;

	player.num_items = 0; 

	for (c = 0; c < num_shiptypes; c++)
	{
		shiptypes[c].hits = hulls[shiptypes[c].hull].hits*256;
		for (s = 0; s < shiptypes[c].num_systems; s++)
			shiptypes[c].sysdmg[s] = 0;
	}

	allies_init();
}

void cs_enemy_init(int32 r)
{
	int32 flt = 0;
	int32 s;

	//r = enemies[rand()%num_enemies];

	sm_fleets[flt].race = r; //enemies[r];
	sm_fleets[flt].num_ships = 0;

	if (r == race_drone)
	{
		sm_fleets[flt].num_ships = 3;
		for (s = 0; s < 3; s++)
			sm_fleets[flt].ships[s] = racefleets[races[sm_fleets[flt].race].fleet].stype[0];
	}

}

void combat_sim()
{
	int end = 0;
	t_player playerback;

	cs_player_init();
	cs_enemy_init(race_drone);

	while (!end && !must_quit)
	{
		if (cs_setupbattle())
		{
			end = 0;
			memcpy(&sm_fleets[1], &sm_fleets[0], sizeof(t_fleet));
			memcpy(&playerback, &player, sizeof(t_player));
			combat(0, 1);
			memcpy(&sm_fleets[0], &sm_fleets[1], sizeof(t_fleet));
			memcpy(&player, &playerback, sizeof(t_player));
			if (must_quit)
				must_quit = 0;
		}
		else
			end = 1;
	}
}

void cs_insertship(int32 st)
{
	int32 x;
	int32 s;
	int32 t0, t1, t2;
	int32 f = 0;

	t0 = racefleets[races[sm_fleets[f].race].fleet].stype[0];
	t1 = racefleets[races[sm_fleets[f].race].fleet].stype[1];
	t2 = racefleets[races[sm_fleets[f].race].fleet].stype[2];

	// if fighter, add to end
	if (st==0)
	{
		sm_fleets[f].ships[sm_fleets[f].num_ships++] = t0;
		return;
	}

	// if super, add to beginning
	if (st==2)
	{
		for (x = sm_fleets[f].num_ships; x > 0; x--)
			sm_fleets[f].ships[x] = sm_fleets[f].ships[x-1];
		sm_fleets[f].ships[0] = t2;
		sm_fleets[f].num_ships++;
		return;
	}

	// if medium
	s = -1;
	for (x = 0; x < sm_fleets[f].num_ships; x++)
	{
		if (sm_fleets[f].ships[x] != t2)
		{
			s = x; break;			
		}
	}

	if (s==-1)
	{
		sm_fleets[f].ships[sm_fleets[f].num_ships++] = t1;
	}
	else
	{
		for (x = sm_fleets[f].num_ships; x > s; x--)
			sm_fleets[f].ships[x] = sm_fleets[f].ships[x-1];
		sm_fleets[f].ships[s] = t1;
		sm_fleets[f].num_ships++;
	}
}

void cs_delship(int32 s)
{
	int32 x;
	int32 f = 0;

	for (x = s; x < sm_fleets[f].num_ships-1; x++)
		sm_fleets[f].ships[x] = sm_fleets[f].ships[x+1];
	sm_fleets[f].num_ships--;
}

int32 cs_setupbattle()
{
	int32 end;
	int32 c, mc;
	int32 bx=192, by=96, h=256;
	int32 y;
	int32 upd=1;
	int32 mx, my, mo;
	int32 t;
	int32 s;
	int32 race = race_drone;
	int32 pship = 0;
	int32 wm[16];
	int32 nwm = 0;
	int32 f = 0;
	t_ik_image *bg;

	bg = ik_load_pcx("graphics/starback.pcx", NULL);

	if (player.ships[0]>0 && player.ships[0]<4)
		pship = player.ships[0]-1;

	for (c = 0; c < num_shiptypes; c++)
		if (shiptypes[c].flag == 9)
			wm[nwm++] = c;

	start_ik_timer(1, 31);
	while (get_ik_timer(1) < 2 && !must_quit)
	{
		ik_eventhandler();
	}
	Play_Sound(WAV_MUS_TITLE, 15, 1, 100, 22050,-1000);
	while (get_ik_timer(1) < 4 && !must_quit)
	{
		ik_eventhandler();
	}
	Play_Sound(WAV_MUS_TITLE, 14, 1, 80, 22050, 1000);
		
	end = 0; t = get_ik_timer(2);
	while (!end && !must_quit)
	{
		ik_eventhandler();
		c = ik_inkey();
		mc = ik_mclick();
		mx = ik_mouse_x - bx; 
		my = ik_mouse_y - by;

		y = t;
		t = get_ik_timer(2);
		if (t != y)
			upd = 1;

		if (sm_fleets[f].num_ships > 0)
			if (c==13 || c==32)
				end = 2;

		if ((mc & 1) && mx > 0 && mx < 240)
		{
			if (my > h-24 && my < h-8) // buttons
			{
				if (mx > 16 && mx < 64) // cancel
				{	end = 1; Play_SoundFX(WAV_DOT); }
				else if (mx > 176 && mx < 224) // ok
					if (sm_fleets[f].num_ships > 0)
					{	end = 2; Play_SoundFX(WAV_DOT); }
			}
			if (my > CS_SHIP+12 && my < CS_SHIP+44)
			{
				pship = MIN(2,(mx - 12)/72);
				player.ships[0] = pship+1;
				Play_SoundFX(WAV_DOT);
			}
			if (my > CS_WING+11 && my < CS_WING+30 && mx > 15 && mx < 15+nwm*21 && player.num_ships < 3)
			{
				c = wm[(mx-15)/21];
				if (player.num_ships < 2 || (shiptypes[c].flag & 8))
					player.ships[player.num_ships++] = c;
				else if (player.ships[1] != c)
					player.ships[player.num_ships++] = c;
				Play_SoundFX(WAV_DOT);
			}
			if (my > CS_WING+36 && my < CS_WING+100 && mx > 48 && mx < 48+(player.num_ships-1)*72)
			{
				if (mx < 120)
					player.ships[1] = player.ships[2];
				player.num_ships--;
				Play_SoundFX(WAV_DOT);
			}
			if (my > CS_FLEET+7 && my < CS_FLEET+26)
			{
				if (mx > 15 && mx < 74 && sm_fleets[f].num_ships<12)
				{
					c = racefleets[races[race].fleet].stype[(mx-14)/20];
					cs_insertship((mx-14)/20);
					Play_SoundFX(WAV_DOT);
				}
				if (mx > 77 && mx < 222)
				{
					if ((mx - 78)/12 < sm_fleets[f].num_ships)
					{	cs_delship((mx-78)/12); Play_SoundFX(WAV_DOT); }
				}

			}
		}

		if (upd)
		{
			upd = 0;
			prep_screen();
			ik_copybox(bg, screen, 0, 0, 640, 480, 0,0);

			interface_drawborder(screen, bx, by, bx+240, by+h, 1, STARMAP_INTERFACE_COLOR, textstring[STR_COMBAT_SIMTITLE]);

			y = by+CS_SHIP;
			ik_print(screen, font_6x8, bx+16, y, STARMAP_INTERFACE_COLOR, textstring[STR_COMBAT_SIMSHIP], shiptypes[pship+1].name);
			for (c = 0; c < 3; c++)
			{
				ik_dsprite(screen, bx+16+c*72, y+12, spr_IFdifenemy->spr[c+3], 0);
				ik_dsprite(screen, bx+16+c*72, y+12, spr_IFborder->spr[IF_BORDER_FLAT], 2+(((pship==c)*3)<<8));
			}

			mo = -1;
			if (my > CS_WING+11 && my < CS_WING+30 && mx > 15 && mx < 15+nwm*21)
				mo = (mx-15)/21;
			if (my > CS_WING+36 && my < CS_WING+100 && mx > 48 && mx < 48+(player.num_ships-1)*72)
			{
				for (mo = 0; mo < nwm; mo++)
					if (wm[mo] == player.ships[(mx-48)/72+1])
						break;
				if (mo >= nwm)
					mo = -1;
			}
			y = by+CS_WING;
			if (mo==-1)
				ik_print(screen, font_6x8, bx+16, y, STARMAP_INTERFACE_COLOR, textstring[STR_COMBAT_SIMWINGMEN]);
			else
				ik_print(screen, font_6x8, bx+16, y, STARMAP_INTERFACE_COLOR, "%s: %s", textstring[STR_COMBAT_SIMWINGMEN], shiptypes[wm[mo]].name);
			for (c = 0; c < nwm; c++)
			{
				s = wm[c];
				interface_thinborder(screen, bx+16+c*21, y+11, bx+35+c*21, y+30, 3*(mo==c)+STARMAP_INTERFACE_COLOR*(mo!=c), 0);
				ik_drsprite(screen, bx+25+c*21, y+20, 0, 16, hulls[shiptypes[s].hull].sprite, 0);
			}
			for (c = 0; c < 2; c++)
			{
				if (c < player.num_ships-1)
				{
					s = (mo>-1 && wm[mo]==player.ships[c+1]);
					ik_dsprite(screen, bx+52+c*72, y+36, hulls[shiptypes[player.ships[c+1]].hull].sprite, 4);
					ik_dsprite(screen, bx+52+c*72, y+36, spr_IFborder->spr[IF_BORDER_PORTRAIT], 2+(((1-s)*STARMAP_INTERFACE_COLOR+s*3)<<8));
				}
				else
					interface_thinborder(screen, bx+52+c*72, y+36, bx+116+c*72, y+100, STARMAP_INTERFACE_COLOR, STARMAP_INTERFACE_COLOR*16+2);
			}

			y = by+CS_FLEET;
			mo = -1;
			if (my > CS_FLEET+7 && my < CS_FLEET+26 && mx > 15 && mx < 74)
			{
				mo = (mx-14)/20;
				ik_print(screen, font_6x8, bx+16, y+32, STARMAP_INTERFACE_COLOR, shiptypes[racefleets[races[race].fleet].stype[mo]].name);
			}
			ik_print(screen, font_6x8, bx+16, y-4, STARMAP_INTERFACE_COLOR, textstring[STR_COMBAT_SIMENEMIES]);

			interface_thinborder(screen, bx+75, y+7, bx+224, y+26, STARMAP_INTERFACE_COLOR, 0);
			for (c = 0; c < 3; c++)
			{
				s = racefleets[races[race].fleet].stype[c];
				interface_thinborder(screen, bx+15+c*20, y+7, bx+34+c*20, y+26, (mo!=c)*STARMAP_INTERFACE_COLOR+(mo==c)*3, 0);
				ik_drsprite(screen, bx+24+c*20, y+16, 0, 16, hulls[shiptypes[s].hull].sprite, 0);
			}
			for (c = 0; c < sm_fleets[f].num_ships; c++)
			{
				ik_drsprite(screen, bx+83+c*12, y+16, 0, 16, hulls[shiptypes[sm_fleets[0].ships[c]].hull].sprite, 0);
			}

			interface_drawbutton(screen, bx+16, by+h-24, 48, STARMAP_INTERFACE_COLOR, textstring[STR_CANCEL]);
			interface_drawbutton(screen, bx+240-64, by+h-24, 48, STARMAP_INTERFACE_COLOR*(sm_fleets[f].num_ships>0), textstring[STR_START]);

			ik_blit();
		}

	}

	Stop_Sound(14);
	Stop_Sound(15);

	player.ships[0] = pship + 1;

	if (must_quit)
		end = 1;


	del_image(bg);

	return end-1;
}