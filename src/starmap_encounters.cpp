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

// ----------------
//		CONSTANTS
// ----------------

// ----------------
// GLOBAL VARIABLES
// ----------------

// ----------------
// LOCAL VARIABLES
// ----------------

extern int32 kawangi_score;
extern int32 kawangi_splode;


// ----------------
// LOCAL PROTOTYPES
// ----------------

// ----------------
// GLOBAL FUNCTIONS
// ----------------

// normal exploration

int32 starmap_entersystem()
{
	int32 c;
	int32 m;
	int32 ret = 1;

	if (sm_stars[player.system].novadate>0 && 
			player.stardate>=sm_stars[player.system].novadate && 
			player.stardate<=sm_stars[player.system].novadate + 120)
	{
		player.death = 4;
		player.num_ships = 0;
		return 0;
	}

	starmap_sensefleets();

	for (c=0;c<STARMAP_MAX_FLEETS;c++)
	if (sm_fleets[c].system == player.system && sm_fleets[c].enroute==0 && sm_fleets[c].num_ships > 0)
	{
		m = 0;
		if (races[sm_fleets[c].race].met == 2)
			m = 1;
		else if (fleet_encounter(c))
			m = 1;

		if (m)
		{
			m = races[sm_fleets[c].race].met;
			starmap_meetrace(sm_fleets[c].race);
#ifndef DEMO_VERSION
			starmap_mantle(c);
			if (sm_fleets[c].race == race_muktian)
			{
				if (muktian_encounter()) // agree to leave -> "flee"
				{	sm_fleets[c].explored = 2; return 0; }
			}
#endif
			if (sm_fleets[c].race == race_klakar && m < 2)
			{
				sm_fleets[c].explored = 2;
				klakar_encounter();
			}

			if (races[sm_fleets[c].race].met < 2)
			{
				if (m == 0)
					enemy_encounter(sm_fleets[c].race);
				must_quit = 0;
				combat(c, 0);
				player.sel_ship = 0;
				if (sm_fleets[c].num_ships>0)
					ret = 0;
			}
		}
		else	// flee
		{			
			ret = 0;
		}
	}

	return ret;
}

void starmap_exploreplanet()
{
	int32 mc, c, ti, ot;
	int32 h, n;
	int32 s, sh;
	int32 end = 0;
	int32 it = -1, r = -1;
	int32 bx = 224, by = 112;
	int32 i[2], cu=0;
	int32 cname=1;
	char name[32];
	char hisher[8];
	char texty[256];


	if (sm_stars[player.target].explored == 2)
		return;

	if (sm_stars[player.target].planet == 10)
	{	cname = 0; }

	Stop_All_Sounds();

	halfbritescreen();

	if (sm_stars[player.target].explored)
		cname=0;
	sm_stars[player.target].explored = 2;
	c = sm_stars[player.target].card; 
//	card_display(c);

	h = 216;
	if ((ecards[c].type == card_item) || (ecards[c].type == card_rareitem) || (ecards[c].type == card_lifeform))
	{
		it = ecards[c].parm; //-1;
		if (it > -1)
		{
			starmap_additem(it, 0);
			if (itemtypes[it].type == item_weapon || itemtypes[it].type == item_system)
				starmap_tutorialtype = tut_upgrade;
			else if (itemtypes[it].type == item_device)
				starmap_tutorialtype = tut_device;
			else 
				starmap_tutorialtype = tut_treasure;
		}
	}
	else if (ecards[c].type == card_ally)
	{
		r = shiptypes[ecards[c].parm].race;
#ifndef DEMO_VERSION
		if (r == race_none || r == race_muktian)
#else
		if (r == race_none)
#endif
			starmap_advancedays(7);
		player.ships[player.num_ships] = ecards[c].parm;
		player.sel_ship = 0; //player.num_ships;
		starmap_tutorialtype = tut_ally;

		player.num_ships++;
		ik_print_log("Found Ally %s\n", shiptypes[player.ships[player.num_ships-1]].name);
		for (n = 0; n < shiptypes[player.ships[player.num_ships-1]].num_systems; n++)
		{
			ik_print_log("%s\n", shipsystems[shiptypes[player.ships[player.num_ships-1]].system[n]].name);
		}
	}
	else if (ecards[c].type == card_event)
	{
		player.bonusdata += ecards[c].parm;
#ifndef DEMO_VERSION
		if (!strcmp(ecards[c].name, textstring[STR_EVENT_CONE]) || !strcmp(ecards[c].name, textstring[STR_EVENT_HULK]))
			h += 32;
#endif
	}

	prep_screen();
	interface_drawborder(screen,
											 bx, by, bx+192, by+h,
											 1, STARMAP_INTERFACE_COLOR, textstring[STR_CARD_PLANET]);


	ik_print(screen, font_6x8, bx+16, by+24, 3, sm_stars[player.target].planetname);
	if (cname)
		ik_print(screen, font_4x8, bx+176-strlen(textstring[STR_CARD_RENAME])*4, by+24, 3, textstring[STR_CARD_RENAME]);
	ik_dsprite(screen, bx+16, by+36, spr_SMplanet2->spr[sm_stars[player.target].planetgfx], 0);
	ik_dsprite(screen, bx+16, by+36, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
	interface_textbox(screen, font_6x8,
										bx+84, by+36, 96, 104, 0,
										platypes[sm_stars[player.target].planet].text);

	sprintf(texty, ecards[c].text, "A", "B");
	if (ecards[c].type == card_event)
	{	
		sprintf(name, textstring[STR_CARD_EVENT]);
#ifndef DEMO_VERSION
		if (!strcmp(ecards[c].name, textstring[STR_EVENT_DEVA]))
		{
			Play_Sound(WAV_NOPLANET, 15, 1);
			sprintf(texty, ecards[c].text, sm_stars[player.target].starname);
		}
#endif
		if (!strcmp(ecards[c].name, textstring[STR_EVENT_FLARE]))
		{
			sh = 0;
			if (player.num_ships > 1)
			{
				sh = -1;
				while (sh == -1)
				{
					sh = rand()%player.num_ships;
					if (shiptypes[player.ships[sh]].flag & 32)
						sh = -1;
				}
			}
	
			sh = player.ships[sh];
			Play_Sound(WAV_FLARE, 15, 1);
			s = 0;
			for (n = 0; n < shiptypes[sh].num_systems; n++)
			{
				if (shipsystems[shiptypes[sh].system[n]].item > -1)
					s++;
			}

			if (s > 0)
			{
				s = rand()%s;
				r = 0;
				for (n = 0; n < shiptypes[sh].num_systems; n++)
				{
					if (shipsystems[shiptypes[sh].system[n]].item > -1)
					{	
						if (r == s)
						{	s = n; break; }
						r++; 
					}
				}
				sprintf(texty, ecards[c].text, 
								shiptypes[sh].name, 
								shipsystems[shiptypes[sh].system[s]].name);
				shiptypes[sh].sysdmg[s]=1;
				//starmap_uninstallsystem(it, 1);
			}
			else
			{
				sprintf(texty, ecards[c].text2, shiptypes[sh].name); // player.shipname);
			}
			
		}
		else if (!strcmp(ecards[c].name, textstring[STR_EVENT_THIEF]))
		{
			Play_Sound(WAV_SPY, 15, 1);
			s = -1;
			for (n = 0; n < player.num_items; n++)
			{
				if (itemtypes[player.items[n]].type != item_lifeform && itemtypes[player.items[n]].flag != 7)
					s = 1;
			}
			if (s>-1)
			{
				s = rand()%player.num_items;
				while (itemtypes[player.items[s]].type == item_lifeform || itemtypes[player.items[s]].flag == 7)
					s = rand()%player.num_items;
				n = player.items[s];
				starmap_removeitem(s);
				sprintf(texty, ecards[c].text, itemtypes[n].name);
				if (rand()&1)
					kla_items[kla_numitems++]=n;
			}
			else
				sprintf(texty, ecards[c].text2, player.shipname);
		}
#ifndef DEMO_VERSION
		else if (!strcmp(ecards[c].name, textstring[STR_EVENT_NOVA]))
		{
			Play_Sound(WAV_NOVA, 15, 1);
			sprintf(texty, ecards[c].text, sm_stars[player.system].starname);
			sm_stars[player.system].novadate = player.stardate+30;
			sm_stars[player.system].novatype = 0;
			for (n = 0; n < STARMAP_MAX_FLEETS; n++)
			if (sm_fleets[n].race == race_klakar && sm_fleets[n].num_ships>0)
			{
				r = get_distance(sm_stars[sm_fleets[n].system].x - sm_stars[player.system].x,
												 sm_stars[sm_fleets[n].system].y - sm_stars[player.system].y);
				if (r < 100)	// move klakar to safety
				{
					s = -1;
					while (s == -1 && !must_quit)
					{
						ik_eventhandler();
						s = rand()%num_stars;
						r = get_distance(sm_stars[s].x - sm_stars[player.system].x,
														 sm_stars[s].y - sm_stars[player.system].y);
						if (r < 100)
							s = -1;
						else if (s == homesystem)
							s = -1;
						else 
							for (r = 0; r < STARMAP_MAX_FLEETS; r++)
								if (sm_fleets[r].num_ships>0 && r!=n && sm_fleets[r].system==s)
									s = -1;
					}
					if (s > -1)
						sm_fleets[n].system = s;
				}
			}
		}
		else if (!strcmp(ecards[c].name, textstring[STR_EVENT_SABOT]))
		{
			Play_Sound(WAV_SABOTEUR, 15, 1);
			s = -1;
			if (shiptypes[0].num_systems > 0)
			{
				for (r = 0; r < shiptypes[0].num_systems; r++)
				if (shipsystems[shiptypes[0].system[r]].item > -1)
					s = 1;
			}
			if (s > -1)
			{
				s = -1;
				while (s == -1)
				{
					s = rand()%shiptypes[0].num_systems;
					if (shipsystems[shiptypes[0].system[s]].item == -1)
						s = -1;
				}
				sprintf(texty, ecards[c].text, 
								shipsystems[shiptypes[0].system[s]].name);
				starmap_destroysystem(s);
			}
			else
			{
				sprintf(texty, ecards[c].text2, 
								player.shipname);
				player.death = 5;
				player.num_ships = 0;
			}
		}
		else if (!strcmp(ecards[c].name, textstring[STR_EVENT_WHALE]))
		{
			Play_Sound(WAV_WHALES, 15);
		}
		else if (!strcmp(ecards[c].name, textstring[STR_EVENT_GIANT]))
		{
			Play_Sound(WAV_GASGIANT, 15, 1);
		}
		else if (!strcmp(ecards[c].name, textstring[STR_EVENT_CONE]))
		{
			Play_Sound(WAV_CUBE, 15, 1);

			ik_print(screen, font_4x8, bx+16, by+h-48, 0, textstring[STR_EVENT_CONE2]);
			for (s = 0; s < 2; s++)
			{
				n=0;
				while (!n && !must_quit)
				{
					ik_eventhandler();
					n = 1;
					i[s]=rand()%num_itemtypes;
					if (s==1 && i[s]==i[0])
						n=0;
					else if (itemtypes[i[s]].flag & 1)
						n=0;
					else {
						for (r=0;r<num_stars;r++)
						if (ecards[sm_stars[r].card].type>=card_item)	// && !sm_stars[r].explored
							if (ecards[sm_stars[r].card].parm == i[s])
								n=0;
					}
					if (n)
					{	for (r=0;r<kla_numitems;r++)
						if (kla_items[r]==i[s])
							n=0;
					}
					if (n)
					{	for (r=0;r<player.num_items;r++)
						if (player.items[r]==i[s])
							n=0;
					}
					if (n)
					{	for (r=0;r<shiptypes[0].num_systems;r++)
							if (shipsystems[shiptypes[0].system[r]].item>-1)
								if (shipsystems[shiptypes[0].system[r]].item == i[s])
									n=0;
					}
				}
			}
			starmap_additem(i[0], 0);
			starmap_additem(i[1], 0);
			ik_print(screen, font_6x8, bx+16, by+h-40, 3, itemtypes[i[0]].name);
			ik_print(screen, font_6x8, bx+16, by+h-32, 3, itemtypes[i[1]].name);
			cu = 1; r = -r;
		}
		else if (!strcmp(ecards[c].name, textstring[STR_EVENT_HULK]))
		{
			Play_Sound(WAV_SPACEHULK, 15, 1);

			ik_print(screen, font_4x8, bx+16, by+h-48, 0, textstring[STR_EVENT_HULK2]);
			for (s = 0; s < 2; s++)
			{
				n=0;
				while (!n && !must_quit)
				{
					ik_eventhandler();
					n = 1;
					i[s]=rand()%num_itemtypes;
					if (s==1 && i[s]==i[0])
						n=0;
					else if (itemtypes[i[s]].flag & 1)
						n=0;
					else {
						for (r=0;r<num_stars;r++)
						if (ecards[sm_stars[r].card].type>=card_item) // && !sm_stars[r].explored
							if (ecards[sm_stars[r].card].parm == i[s])
								n=0;
					}
					if (n)
					{	for (r=0;r<kla_numitems;r++)
						if (kla_items[r]==i[s])
							n=0;
					}
					if (n)
					{	for (r=0;r<player.num_items;r++)
						if (player.items[r]==i[s])
							n=0;
					}
					if (n)
					{	for (r=0;r<shiptypes[0].num_systems;r++)
							if (shipsystems[shiptypes[0].system[r]].item>-1)
								if (shipsystems[shiptypes[0].system[r]].item == i[s])
									n=0;
					}
				}
			}

			starmap_additem(i[0], 0);
			starmap_additem(i[1], 0);
			ik_print(screen, font_6x8, bx+16, by+h-40, 3, itemtypes[i[0]].name);
			ik_print(screen, font_6x8, bx+16, by+h-32, 3, itemtypes[i[1]].name);
			cu = 1; r = -r;
		}
#endif
	}
	else if (ecards[c].type == card_ally)
	{
		sprintf(name, textstring[STR_CARD_ALLY]);
	}
	else
	{	
		sprintf(name, textstring[STR_CARD_DISCOVERY], itemtypes[it].clas); 
		for (n = 0; n < (int32)strlen(name); n++)
		{
			if (name[n]>='a' && name[n]<='z')
				name[n]-='a'-'A';
		}
	}
	ik_print(screen, font_6x8, bx+96-strlen(name)*3, by+108, STARMAP_INTERFACE_COLOR, name);
	ik_print(screen, font_6x8, bx+16, by+120, 3, ecards[c].name);
 	if (it == -1)
	{
		interface_textbox(screen, font_6x8,
											bx+16, by+128, 160, 64, 0,
											texty); //ecards[c].text);
	}
	else
	{
		interface_textbox(screen, font_6x8,
											bx+16, by+128, 160, 64, 0,
											itemtypes[it].text);
		if (itemtypes[it].sound<0)
			Play_Sound(-itemtypes[it].sound, 15, 1);
		else
			Play_Sound(itemtypes[it].sound, 15);
	}

	interface_drawbutton(screen, bx+144, by+h-24, 32, STARMAP_INTERFACE_COLOR, textstring[STR_OK]);

	ik_blit();

	if (settings.random_names & 4)
	{
		interface_tutorial(tut_explore);
	}

	if (ecards[c].type == card_ally)
	{
		starmap_meetrace(r);
		prep_screen();
		ik_blit();
		Play_Sound(WAV_ALLY, 15, 1);
	}

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	

		ot = ti; ti = get_ik_timer(2);
		if (ti != ot)
		{ prep_screen(); ik_blit();	}

		if (mc == 1)
		{
			if (cname)
			if (ik_mouse_y > by+24 && ik_mouse_y < by+32 && ik_mouse_x > bx+16 && ik_mouse_x < bx+176)
			{
				strcpy(name, sm_stars[player.target].planetname);
				prep_screen();
				ik_drawbox(screen, bx+16, by+24, bx+176, by+32, STARMAP_INTERFACE_COLOR*16+3);
				free_screen();
				ik_text_input(bx+16, by+24, 16, font_6x8, "", name, STARMAP_INTERFACE_COLOR*16+3, STARMAP_INTERFACE_COLOR);
				if (strlen(name))
					strcpy(sm_stars[player.target].planetname, name);

				prep_screen();
				ik_drawbox(screen, bx+16, by+24, bx+176, by+32, STARMAP_INTERFACE_COLOR*16+3);
				ik_print(screen, font_6x8, bx+16, by+24, 3, sm_stars[player.target].planetname);

				ik_blit();
			}

			if (ik_mouse_y > by+h-24 && ik_mouse_y < by+h-8 && ik_mouse_x > bx+144 && ik_mouse_x < bx+176)
			{	end = 1; Play_SoundFX(WAV_DOT, get_ik_timer(0)); }
			if (cu)
			{
				if (ik_mouse_x > bx+16 && ik_mouse_x < bx+176 && ik_mouse_y > by+h-40 && ik_mouse_y < by+h-24)
				{
					Play_SoundFX(WAV_INFO);
					interface_popup(font_6x8, bx+16, by+h-112, 192, 96, STARMAP_INTERFACE_COLOR, 0, 
												itemtypes[i[(ik_mouse_y-(by+h-40))/8]].name, itemtypes[i[(ik_mouse_y-(by+h-40))/8]].text);
				}				
			}
		}

//		if (c == 13 || mc == 1)
//			end = 1;
	}

	ik_print_log("Exploring the star system, discovered a %s planet and named it %s.\n", 
								platypes[sm_stars[player.target].planet].name,
								sm_stars[player.target].planetname);

	must_quit = 0;

	if (ecards[c].type == card_ally)
	{
		Stop_Sound(15);
#ifndef DEMO_VERSION
		if (r==race_terran || r==race_zorg || r==race_garthan)	// terran / zorg / garthan mercs
#else
		if (r==race_terran || r==race_garthan)	// terran / zorg / garthan mercs
#endif
		{
			if (shiptypes[ecards[c].parm].flag & 64)
				sprintf(hisher, textstring[STR_MERC_HER]);
			else
				sprintf(hisher, textstring[STR_MERC_HIS]);

			sprintf(texty, textstring[STR_MERC_PAYMENT], 
							hulls[shiptypes[ecards[c].parm].hull].name,
							shiptypes[ecards[c].parm].name,
							hisher);
			n = pay_item(textstring[STR_MERC_BILLING], texty, r);

			if (n == -1)
			{
				starmap_removeship(player.num_ships-1);
				starmap_tutorialtype = tut_starmap;
				ik_print_log("Ally cancelled\n");
			}
			else
				sm_stars[player.system].card = 0;
		}
		else
			sm_stars[player.system].card = 0;
	}
	else if (ecards[c].type == card_lifeform)
	{
#ifndef DEMO_VERSION
		if (itemtypes[it].flag & lifeform_hard)
		{
			h = itemtypes[it].cost/10 + rand()%(itemtypes[it].cost/10);
			sprintf(texty, textstring[STR_LIFEFORM_HARD], itemtypes[it].name, h);
			if (!interface_popup(font_6x8, bx+16, by+96, 192, 0, STARMAP_INTERFACE_COLOR, 0, textstring[STR_LIFEFORM_HARDT], texty, textstring[STR_YES], textstring[STR_NO]))
			{
				starmap_advancedays(h);
				sm_stars[player.system].card = 0;
			}
			else
				starmap_removeitem(player.num_items-1);
		}
		else
#endif
			sm_stars[player.system].card = 0;
	}



	Stop_Sound(15);

	reshalfbritescreen();
}

int32 starmap_explorehole(int32 h, int32 t)
{
	int32 mc, c;
	int32 end = 0;
	int32 bx = SM_MAP_X+136, by = SM_MAP_Y+48+240*(sm_holes[h].y>0), z;
	int32 mx, my;
	char name[32];

	if (sm_holes[h].explored)
		return 1;

	if (!sm_holes[h].size)
		return 1;

	// remove holes on first move
	if (player.system == homesystem)
	{
		sm_holes[h].size = 0;
		return 1;
	}

	// remove holes if escaping a nova
	if (sm_stars[player.system].novadate > 0 &&
			sm_stars[player.system].novatype == 0 &&
			player.stardate < sm_stars[player.system].novadate + 365*4)
	{
		sm_holes[h].size = 0;
		return 1;
	}

	player.explore = h+1;
	sm_holes[h].explored = 1;
	player.bonusdata += 200;

	z = 148;
	Play_Sound(WAV_BLACKHOLE, 15, 1);

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	
		mx = ik_mouse_x - bx; my = ik_mouse_y - by;

		if (must_quit)
		{
			end = 1;
			must_quit = 0;
		}

		c = t; t = get_ik_timer(0);

		if (t != c)
		{
			prep_screen();
			starmap_display(t);

			interface_drawborder(screen,
													 bx, by, bx+208, by+z,
													 1, STARMAP_INTERFACE_COLOR, textstring[STR_BLACKHOLE_TITLE]);
			ik_drawbox(screen, bx+16, by+40, bx+79, by+103, 0);
			ik_drsprite(screen, bx+48, by+72, 1023-((t*2)&1023), 64, spr_SMstars->spr[8], 4);
			ik_dsprite(screen, bx+16, by+40, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
			ik_print(screen, font_6x8, bx+16, by+24, 3, sm_holes[h].name);
			ik_print(screen, font_4x8, bx+192-strlen(textstring[STR_CARD_RENAME])*4, by+24, 3, textstring[STR_CARD_RENAME]);
			interface_textbox(screen, font_4x8,
												bx+88, by+40, 104, 64, 0,
												textstring[STR_BLACKHOLE_DESC]);
			ik_print(screen, font_4x8, bx+16, by+112, 0, textstring[STR_BLACKHOLE_WARN]);
			interface_drawbutton(screen, bx+16, by+z-24, 64, STARMAP_INTERFACE_COLOR, textstring[STR_GOBACK]);
			interface_drawbutton(screen, bx+208-80, by+z-24, 64, STARMAP_INTERFACE_COLOR, textstring[STR_CONTINUE]);
			ik_blit();
		}

		if (mc == 1)
		{
			if (my > 24 && my < 32) // rename
			{
				strcpy(name, sm_holes[h].name);
				prep_screen();
				ik_drawbox(screen, bx+16, by+24, bx+192, by+32, STARMAP_INTERFACE_COLOR*16+3);
				free_screen();
				ik_text_input(bx+16, by+24, 16, font_6x8, "", name, STARMAP_INTERFACE_COLOR*16+3, STARMAP_INTERFACE_COLOR);
				if (strlen(name))
					strcpy(sm_holes[h].name, name);
			}
			if (my > z-24 && my < z-8)
			{
				if (mx > 208-80 && mx < 192)
				{	end = 2; Play_SoundFX(WAV_ACCEPT, get_ik_timer(0)); }
				if (mx > 16 && mx < 80)
				{	end = 1; Play_SoundFX(WAV_DECLINE, get_ik_timer(0)); }
			}
		}
	}
	/*
	ik_print_log("Discovered an uncharted black hole and named it %s.");
	if (end==2)
		ik_print_log("Determined the singularity was not close enough to endanger the %s and stayed on course toward %s system.\n",
									player.shipname, sm_stars[player.target].starname);
	else
		ik_print_log("The proximity of the black hole to our plotted course posed a danger to %s and forced us to turn back.\n",
									player.shipname);
*/
	player.explore = 0;
	Stop_Sound(15);

	return end-1;	
}

// alien encounters


int32 fleet_encounter(int32 flt, int32 inc)
{
	int32 mc, c;
	int32 end = 0;
	int32 bx = 240, by = 152;
	int32 mx, my;
	int32 r, t, t0, st;
	int32 l;
	int32 s;
	int32	upd = 1;
	int32 sx[16], sy[16];
	char texty[32];

	halfbritescreen();

	r = sm_fleets[flt].race;

	if (!sm_fleets[flt].explored)
		sm_fleets[flt].explored = 1;

	Stop_All_Sounds();

	t = sm_fleets[flt].num_ships;
	for (c = 0; c < t; c++)
	{
		sx[c] = bx + 32 + (c*96+24+rand()%48)/t;
		sy[c] =	by + 56 + rand()%16;
	}

	Play_Sound(WAV_RADAR, 15, 1);

	st = t = get_ik_timer(0);
	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	
		t0 = t; t = get_ik_timer(0);
		mx = ik_mouse_x - bx; my = ik_mouse_y - by;
		if (mc == 1 && mx > 16 && mx < 64 && my > 160 && my < 176)
		{	end = 1; Play_SoundFX(WAV_DECLINE, get_ik_timer(0)); }
		if (mc == 1 && mx > 96 && mx < 144 && my > 160 && my < 176)
		{	end = 2; Play_SoundFX(WAV_ACCEPT, get_ik_timer(0)); }
		if (must_quit)
		{
			end = 1;
			must_quit = 0;
		}

		if (t > t0 || upd == 1)
		{
			upd = 0;
			prep_screen();
			if (!inc)
				sprintf(texty, textstring[STR_SCANNER_ALIENS]);
			else
				sprintf(texty, textstring[STR_SCANNER_INCOMING]);
			interface_drawborder(screen,
													 bx, by, bx+160, by+184,
													 1, STARMAP_INTERFACE_COLOR, texty);
			if (races[r].met)
			{
				sm_fleets[flt].explored = 2;
				ik_print(screen, font_6x8, bx+16, by+24, 3, textstring[STR_SCANNER_RACE], races[r].name);
			}
			else
				ik_print(screen, font_6x8, bx+16, by+24, 3, textstring[STR_SCANNER_NORACE]);

			ik_dsprite(screen, bx+16, by+32, spr_IFborder->spr[19], 2+(4<<8));
			l = 10 + 5 * (((t-st)%50)<10);
			for (c = 0; c < sm_fleets[flt].num_ships; c++)
			{
				s = hulls[shiptypes[sm_fleets[flt].ships[c]].hull].size;
				ik_drsprite(screen, sx[c], sy[c], 0, 8,
									 spr_IFarrows->spr[10-(s<32)+(s>80)], 5+(l<<8));
			}
			l = ((t&63)*3 - 95 + 1024) & 1023;
			ik_dspriteline(screen, bx + 80, by + 152, 
										bx + 80 + ((sin1k[l] * 112) >> 16), 
										by + 152 - ((cos1k[l] * 112) >> 16), 
										8, (t&31), 18, spr_weapons->spr[2], 5+(10<<8));
			if (!inc)
			{
				interface_drawbutton(screen, bx+16, by+160, 48, STARMAP_INTERFACE_COLOR, textstring[STR_SCANNER_AVOID]);
				interface_drawbutton(screen, bx+96, by+160, 48, STARMAP_INTERFACE_COLOR, textstring[STR_SCANNER_ENGAGE]);
			}
			else
			{
				interface_drawbutton(screen, bx+16, by+160, 48, STARMAP_INTERFACE_COLOR, textstring[STR_SCANNER_FLEE]);
				interface_drawbutton(screen, bx+96, by+160, 48, STARMAP_INTERFACE_COLOR, textstring[STR_SCANNER_ENGAGE]);
			}
			ik_blit();

			if (settings.random_names & 4)
			{
				interface_tutorial(tut_encounter);
			}
//			if (t/50 > t0/50)
//				Play_Sound(WAV_RADAR, 15, 1);

		}

	}

	Stop_Sound(15);

	reshalfbritescreen();
	return end - 1;
}

void starmap_meetrace(int32 r)
{
	int32 mc, c;
	int32 end = 0;
	int32 t;
	int32 bx = 216, by = 152;
	int32 mx, my;

	if (races[r].met)
		return;

	if (r < race_klakar)
		return;

	if (r >= race_unknown) 
		return;

	races[r].met = 1;

	halfbritescreen();

	prep_screen();
	interface_drawborder(screen,
											 bx, by, bx+208, by+152,
											 1, STARMAP_INTERFACE_COLOR, textstring[STR_ALIEN_CONTACT]);
	ik_print(screen, font_6x8, bx+104-strlen(races[r].name)*3, by+26, 3, races[r].name);
	interface_textbox(screen, font_4x8,
										bx+88, by+40, 104, 64, 0,
										races[r].text);
	ik_dsprite(screen, bx+16, by+40, spr_SMraces->spr[r], 0);
	ik_dsprite(screen, bx+16, by+40, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
	ik_print(screen, font_4x8, bx+16, by+112, 0, textstring[STR_ALIEN_DEMEANOR], races[r].text2);
	interface_drawbutton(screen, bx+160, by+124, 32, STARMAP_INTERFACE_COLOR, textstring[STR_OK]);

	ik_blit();

	Play_Sound(WAV_KLAKAR + r - race_klakar, 15, 1);

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	
		c = ik_inkey();
		mx = ik_mouse_x - bx; my = ik_mouse_y - by;

		if (mc == 1 && mx > 160 && mx < 192 && my > 124 && my < 140)
		{	end = 1; Play_SoundFX(WAV_DOT, get_ik_timer(0)); }

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ prep_screen(); ik_blit();	}
	}

	Stop_Sound(15);
	must_quit = 0;

	ik_print_log("Made contact with a previously unknown alien race, the %s.\n", races[r].name);

	reshalfbritescreen();
}

void starmap_mantle(int32 flt)
{
#ifndef DEMO_VERSION
	int32 mc, c;
	int32 end = 0;
	int32 bx = 216, by = 152, h = 112;
	int32 mx, my;
	int32 r = sm_fleets[flt].race;
	int32 t;
	char texty[256];

	int32 bab = 0;

	for (c = 0; c < player.num_items; c++)
		if (itemtypes[player.items[c]].flag & device_mantle)
			bab = 1;

	if (bab == 0 && races[r].met == 2)
	{
		if (r != race_klakar && r != race_muktian)
			races[r].met = 1;
		return;
	}

	if (bab == 1 && races[r].met == 2)
	{
		sm_fleets[flt].explored = 2;
	}

	if (bab == 1 && races[r].met != 2)
	{
		if (r == race_muktian || r == race_garthan || r == race_urluquai)
		{
			races[r].met = 2;
			Play_SoundFX(WAV_MANTLE, 0);
			sm_fleets[flt].explored = 2;

			halfbritescreen();

			prep_screen();
			sprintf(texty, textstring[STR_VIDCAST], races[r].name);
			interface_drawborder(screen,
													 bx, by, bx+208, by+h,
													 1, STARMAP_INTERFACE_COLOR, texty);
			switch(r)
			{
				case race_muktian:
				c = STR_MANTLE_MUKTIAN + (rand()&1);
				break;
				case race_garthan:
				c = STR_MANTLE_GARTHAN + (rand()&1);
				break;
				case race_urluquai:
				c = STR_MANTLE_URLUQUAI + (rand()&1);
				break;
				default:
				c = STR_MANTLE_MUKTIAN;
			}

			interface_textbox(screen, font_4x8,
												bx+88, by+24, 104, 64, 0,
												textstring[c]);
			ik_dsprite(screen, bx+16, by+24, spr_SMraces->spr[r], 0);
			ik_dsprite(screen, bx+16, by+24, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
			interface_drawbutton(screen, bx+160, by+h-24, 32, STARMAP_INTERFACE_COLOR, textstring[STR_OK]);

			ik_blit();

			while (!must_quit && !end)
			{
				ik_eventhandler();  // always call every frame
				mc = ik_mclick();	
				c = ik_inkey();
				mx = ik_mouse_x - bx; my = ik_mouse_y - by;

				if (mc == 1 && mx > 160 && mx < 192 && my > h-24 && my < h-8)
				{	end = 1; Play_SoundFX(WAV_DOT, get_ik_timer(0)); }

				c = t; t = get_ik_timer(2);
				if (t != c)
				{ prep_screen(); ik_blit();	}
			}

			ik_print_log("Made contact with a previously unknown alien race, the %s.\n", races[r+2].name);

			reshalfbritescreen();
		}
	}
	must_quit = 0;
#endif

	return;
}

void enemy_encounter(int32 r)
{
	int32 mc, c;
	int32 end = 0;
	int32 bx = 216, by = 152;
	int32 mx, my;
	int32 t;
	char texty[256];

#ifndef DEMO_VERSION
	char *tx;

	if (r != race_garthan && r != race_urluquai && r != race_tanru)
#else
	if (r != race_garthan)
#endif
		return;

	halfbritescreen();

	// generic enemy greeting screen
	prep_screen();
	sprintf(texty, textstring[STR_VIDCAST], races[r].name);
	interface_drawborder(screen,
											 bx, by, bx+208, by+144,
											 1, STARMAP_INTERFACE_COLOR, texty);
	ik_print(screen, font_6x8, bx+16, by+26, 3, textstring[STR_VIDCAST2]);

	if (r == race_garthan)
		sprintf(texty, textstring[STR_GARTHAN_WARN1+rand()%3]);
#ifndef DEMO_VERSION
	else if (r == race_urluquai)
		sprintf(texty, textstring[STR_URLUQUAI_WARN1+rand()%3]);
	else // tan ru
	{
		// generate random tan ru message
		tx = texty; c = 0;
		while (c < 180)
		{
			mx = rand()%3;
			switch (mx)
			{
				case 0:
				*tx++ = 'A'+rand()%('Z'+1-'A');
				*tx++ = 'A'+rand()%('Z'+1-'A');
				*tx++ = 'A'+rand()%('Z'+1-'A');
				*tx++ = ' ';
				c+=4;
				break;

				case 1:
				mc = 1+rand()%8;
				while (mc--)
				{
					*tx++ = '0'+(rand()&1);
					c++;
				}
				*tx++ = ' ';
				c++;
				break;

				case 2:
				default:
				mc = rand()%(strlen(textstring[STR_TANRU_WARN])/4 + 1);
				*tx++ = textstring[STR_TANRU_WARN][mc*4];
				*tx++ = textstring[STR_TANRU_WARN][mc*4+1];
				*tx++ = textstring[STR_TANRU_WARN][mc*4+2];
				*tx++ = ' ';
				c+=4;

			}
		}
		*tx++ = 0;
	}
#endif

	interface_textbox(screen, font_4x8,
										bx+88, by+40, 104, 64, 0,
										texty);

	ik_dsprite(screen, bx+16, by+40, spr_SMraces->spr[r], 0);
	ik_dsprite(screen, bx+16, by+40, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
	interface_drawbutton(screen, bx+160, by+116, 32, STARMAP_INTERFACE_COLOR, textstring[STR_OK]);

	ik_blit();

#ifndef DEMO_VERSION
	Play_Sound(WAV_MESSAGE+(r==race_tanru), 15, 1);
#else
	Play_Sound(WAV_MESSAGE, 15, 1);
#endif

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	
		c = ik_inkey();
		mx = ik_mouse_x - bx; my = ik_mouse_y - by;

		if (mc == 1 && mx > 160 && mx < 192 && my > 116 && my < 132)
		{	end = 1; Play_SoundFX(WAV_DOT, get_ik_timer(0)); }

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ prep_screen(); ik_blit();	}
	}

	Stop_Sound(15);
	must_quit = 0;

	reshalfbritescreen();
	prep_screen();
	ik_blit();
}

#ifndef DEMO_VERSION
int32 muktian_encounter()
{
	int32 mc, c;
	int32 end = 0;
	int32 bx = 216, by = 152;
	int32 mx, my;
	int32 r, m, t;
	int32 o = 0;
	char str[256];

	r = race_muktian;


	m = 0; mc = -1;
	for (c = 0; c < player.num_ships; c++)
	if (shiptypes[player.ships[c]].race == 	r)
	{	
		m = 1; mc = c;
	}

	if (races[r].met == 2 && m == 0)	// if peaceful, continue
		return 0;

	halfbritescreen();

	if (m)
		starmap_removeship(mc);

	prep_screen();
	sprintf(str, textstring[STR_VIDCAST], races[r].name);
	interface_drawborder(screen,
											 bx, by, bx+208, by+144,
											 1, STARMAP_INTERFACE_COLOR, str);
	ik_print(screen, font_6x8, bx+16, by+26, 3, textstring[STR_VIDCAST2]);
	if (m)
	{
		interface_textbox(screen, font_4x8,
											bx+88, by+40, 104, 64, 0,
											textstring[STR_MUKTIAN_THANKS]);
		races[r].met = 2;
		interface_drawbutton(screen, bx+16, by+116, 64, STARMAP_INTERFACE_COLOR, textstring[STR_DECLINE]);
		interface_drawbutton(screen, bx+128, by+116, 64, STARMAP_INTERFACE_COLOR, textstring[STR_ACCEPT]);
	}
	else
	{
		interface_textbox(screen, font_4x8,
											bx+88, by+40, 104, 64, 0,
											textstring[STR_MUKTIAN_WARNING]);
		interface_drawbutton(screen, bx+16, by+116, 64, STARMAP_INTERFACE_COLOR, textstring[STR_LEAVE]);
		interface_drawbutton(screen, bx+128, by+116, 64, STARMAP_INTERFACE_COLOR, textstring[STR_STAY]);
		o = 1;
	}
	ik_dsprite(screen, bx+16, by+40, spr_SMraces->spr[r], 0);
	ik_dsprite(screen, bx+16, by+40, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
//	interface_drawbutton(screen, bx+16, by+116, 64, STARMAP_INTERFACE_COLOR, "DECLINE");
//	interface_drawbutton(screen, bx+128, by+116, 64, STARMAP_INTERFACE_COLOR, "ACCEPT");

	ik_blit();

	Play_Sound(WAV_MESSAGE, 15, 1);

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	
		c = ik_inkey();
		mx = ik_mouse_x - bx; my = ik_mouse_y - by;

		if (must_quit)
		{
			end = 1+o;
			must_quit = 0;
		}

		if (mc == 1 && mx > 16 && mx < 80 && my > 116 && my < 132)
		{	end = 1+o; Play_SoundFX(WAV_DECLINE, get_ik_timer(0));	}
		if (mc == 1 && mx > 128 && mx < 192 && my > 116 && my < 132)
		{	end = 2-o; Play_SoundFX(WAV_ACCEPT, get_ik_timer(0));	}

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ prep_screen(); ik_blit();	}
	}

	Stop_Sound(15);

	reshalfbritescreen();
	prep_screen();
	ik_blit();

	if (m)
	{
		if (end == 2)	// pick up ambassador
		{
			Play_Sound(WAV_MUKTIAN, 15, 1);

			interface_popup(font_6x8, 224, 192, 192, 0, STARMAP_INTERFACE_COLOR, 0, 
											textstring[STR_AMBASSADORT], textstring[STR_AMBASSADOR], textstring[STR_OK]);
			Stop_Sound(15);
			for (c = 0; c < num_itemtypes; c++)
			if (itemtypes[c].flag & lifeform_ambassador) 
			{
				starmap_additem(c, 0);
			}
		}

		return 0;
	}

	return end-1;
}
#endif

void klakar_encounter()
{
	int32 mc, c;
	int32 end = 0;
	int32 bx = 216, by = 152;
	int32 mx, my;
	int32 r, t;
//	t_ik_sprite *bg;

	halfbritescreen();

	r = race_klakar;

	// trader greeting screen
	prep_screen();
//	bg = get_sprite(screen, bx, by, 208, 144);
	interface_drawborder(screen,
											 bx, by, bx+208, by+144,
											 1, STARMAP_INTERFACE_COLOR, textstring[STR_TRADE_TITLE]);
	ik_print(screen, font_6x8, bx+16, by+26, 3, textstring[STR_VIDCAST2]);
	interface_textbox(screen, font_4x8,
										bx+88, by+40, 104, 64, 0,
										textstring[STR_TRADE_MESSAGE]);
	races[r].met = 2;

	ik_dsprite(screen, bx+16, by+40, spr_SMraces->spr[r], 0);
	ik_dsprite(screen, bx+16, by+40, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
	interface_drawbutton(screen, bx+16, by+116, 64, STARMAP_INTERFACE_COLOR, textstring[STR_LEAVE]);
	interface_drawbutton(screen, bx+128, by+116, 64, STARMAP_INTERFACE_COLOR, textstring[STR_TRADE]);

	ik_blit();

	Play_Sound(WAV_MESSAGE, 15, 1);

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	
		c = ik_inkey();
		mx = ik_mouse_x - bx; my = ik_mouse_y - by;

		if (must_quit)
		{
			end = 1;
			must_quit = 0;
		}

		if (mc == 1 && mx > 16 && mx < 80 && my > 116 && my < 132)
		{	end = 1; Play_SoundFX(WAV_DECLINE, get_ik_timer(0)); }
		if (mc == 1 && mx > 128 && mx < 192 && my > 116 && my < 132)
		{	end = 2; Play_SoundFX(WAV_ACCEPT, get_ik_timer(0)); }

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ prep_screen(); ik_blit();	}
	}

	Stop_Sound(15);

//	ik_dsprite(screen, bx, by, bg, 4);

	reshalfbritescreen();

	//free_sprite(bg);

	for (c = 0; c < num_itemtypes; c++)
	if (itemtypes[c].flag & device_beacon)
	{
		starmap_additem(c, 0);
	}

	if (end == 1)
	{
		prep_screen();
		ik_blit();
		return;
	}

	klakar_trade();
}

// kawangi encounter

#ifndef DEMO_VERSION
void kawangi_warning()
{
	int32 mc, c;
	int32 end = 0;
	int32 bx = 216, by = 152, h = 208;
	int32 mx, my;
	int32 t;
	char texty[512];

	halfbritescreen();

	prep_screen();
	interface_drawborder(screen,
											 bx, by, bx+208, by+h,
											 1, STARMAP_INTERFACE_COLOR, "Distress Call");
	interface_textbox(screen, font_4x8,
										bx+16, by+24, 180, 64, 0,
										textstring[STR_KAWANGI_WARNING1]);

	interface_textbox(screen, font_4x8,
										bx+88, by+48, 104, 64, 0,
										textstring[STR_KAWANGI_WARNING2]);

	sprintf(texty, textstring[STR_KAWANGI_WARNING3], player.shipname);
	interface_textbox(screen, font_4x8,
										bx+16, by+112, 176, 80, 0,
										texty);

	ik_dsprite(screen, bx+16, by+44, spr_IFdifnebula->spr[1], 0);
	ik_dsprite(screen, bx+16, by+44, hulls[shiptypes[racefleets[races[race_kawangi].fleet].stype[0]].hull].sprite, 0);
	ik_dsprite(screen, bx+16, by+44, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
	interface_drawbutton(screen, bx+144, by+h-24, 48, STARMAP_INTERFACE_COLOR, textstring[STR_OK]);

	ik_blit();

	Play_Sound(WAV_WARNING, 15, 1);

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	
		c = ik_inkey();
		mx = ik_mouse_x - bx; my = ik_mouse_y - by;

		if (mc == 1 && mx > 144 && mx < 192 && my > h-24 && my < h-8)
		{	end = 1; Play_SoundFX(WAV_DOT); }

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ prep_screen(); ik_blit();	}
	}

	Stop_Sound(15);

	must_quit = 0;
	Play_Sound(WAV_BRIDGE, 15, 1, 50);

	reshalfbritescreen();
}

void kawangi_message(int32 flt, int32 m)
{
	int32 mc, c;
	int32 end = 0;
	int32 bx = 216, by = 176, h = 128;
	int32 mx, my;
	int32 t;
	char texty[512];
	char topic[32];

	halfbritescreen();

	switch (m)
	{
		case 0:	// destroyed
		Play_Sound(WAV_MUS_ROCK, 15, 1);

		if (sm_fleets[flt].explored < 2)	// don't know it's the kawangi
		{
			sprintf(texty, textstring[STR_KAWANGI_KILLED1]);
			sprintf(topic, textstring[STR_KAWANGI_KILLED]);
		}
		else
		{
			sprintf(texty, textstring[STR_KAWANGI_KILLED2]);
			sprintf(topic, races[race_kawangi].name);
		}
		break;

		case 1:	// detect explosion
		Play_Sound(WAV_OPTICALS, 15, 1);

		if (kawangi_score == 0)
		{
			sprintf(texty, textstring[STR_KAWANGI_EXPLO1], sm_stars[sm_fleets[flt].system].starname);
			sprintf(topic, textstring[STR_KAWANGI_EXPLO]);
			kawangi_score++;
		}
		else
		{
			sprintf(texty, textstring[STR_KAWANGI_EXPLO2], sm_stars[sm_fleets[flt].system].starname);
			sprintf(topic, textstring[STR_KAWANGI_EXPLO]);
			kawangi_score++;
		}
		break;
		
		default: ;
	}

	h = interface_textboxsize(font_6x8, 176, 128, texty)*8 + 48;
	by = 224 - h/2;

	prep_screen();

	interface_drawborder(screen,
											 bx, by, bx+208, by+h,
											 1, STARMAP_INTERFACE_COLOR, topic);
	interface_textbox(screen, font_6x8,
										bx+16, by+24, 176, h, 0,
										texty);

	interface_drawbutton(screen, bx+144, by+h-24, 48, STARMAP_INTERFACE_COLOR, textstring[STR_OK]);

	ik_blit();

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	
		c = ik_inkey();
		mx = ik_mouse_x - bx; my = ik_mouse_y - by;

		if (mc == 1 && mx > 144 && mx < 192 && my > h-24 && my < h-8)
		{	end = 1; Play_SoundFX(WAV_DOT); }

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ prep_screen(); ik_blit();	}
	}

	must_quit = 0;
	Stop_Sound(15);
	Play_Sound(WAV_BRIDGE, 15, 1, 50);

	reshalfbritescreen();
}

void starmap_kawangimove(int flt)
{
	int32 c;
	int32 r, rr, cr;
	int32 k;

	rr = -1;
	cr = starmap_stardist(sm_fleets[flt].system, homesystem);
	sm_fleets[flt].target = sm_fleets[flt].system;

	/*
	 taking the shortest route towards hope

		the next star must be closer to hope than current one
		the closer the next star is to current one the better
		(go short hops rather than head straight for hope)

	*/

	if (sm_fleets[flt].system == homesystem) // already at hope
	{
		sm_fleets[flt].target = num_stars;
	}
	else
	{
		for (c = 0; c < num_stars; c++)
		{
			k = 1;
			if (c == sm_fleets[flt].system)
				k = 0;
			else if (ecards[sm_stars[c].card].type == card_rareitem)
				if ((itemtypes[ecards[sm_stars[c].card].parm].flag & device_collapser)>0 && sm_stars[c].explored < 2)
					k = 0;

			if (k)
			{
				r = starmap_stardist(c, homesystem);
				if (r < cr)	// only go to planets that are closer to hope
				{
					r += starmap_stardist(sm_fleets[flt].system, c) * 2;	
					// add the distance to this star from current
					if (r < rr || rr==-1)
					{	
						sm_fleets[flt].target = c;
						rr = r;
					}
				}
			}
		}
	}

	if (sm_fleets[flt].target != sm_fleets[flt].system)	// go kawangi go!
	{
		// blow up system
		if (sm_fleets[flt].system < num_stars && sm_stars[sm_fleets[flt].system].planet != 10)
		{
			kawangi_splode = 1;
		}

		sm_fleets[flt].distance = starmap_stardist(sm_fleets[flt].system, sm_fleets[flt].target);
		sm_fleets[flt].enroute = 1;

		if (sm_fleets[flt].system == homesystem)
			player.death = 6;
	}
}
#endif