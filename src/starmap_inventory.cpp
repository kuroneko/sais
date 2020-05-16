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

// ----------------
// LOCAL PROTOTYPES
// ----------------

// ----------------
// GLOBAL FUNCTIONS
// ----------------

// regular inventory management

void starmap_installitem(int32 n)
{
	int it, sy;
	int ityp, styp;
	int s;
	int d = 0;
	int i;

	hud.invselect = -1;

	it = player.items[n];
	ityp = itemtypes[it].type;
	if (ityp == item_system)
	{
		sy = itemtypes[player.items[n]].index;
		styp = shipsystems[sy].type;
	
		if (styp != sys_misc)
		{	// misc systems don't cancel out
			for (s = 0; s < shiptypes[0].num_systems; s++)
			if (shipsystems[shiptypes[0].system[s]].type == styp)
				starmap_uninstallsystem(s, 0);
		}
		if (shiptypes[0].num_systems < 10)
		{
			d = (int32)(sqrt(itemtypes[it].cost)*.2);
			if (d < 1) d = 1;
			shiptypes[0].sysdmg[shiptypes[0].num_systems] = 0;
			shiptypes[0].system[shiptypes[0].num_systems] = sy;
			shiptypes[0].num_systems++;
			starmap_removeitem(n);
		}
	}
	else if (ityp == item_weapon)
	{
		s = select_weaponpoint();
		if (s > -1)
		{
			if (shiptypes[0].system[s] > -1)
			{	// place old weapon in the inventory
				i = shipsystems[shiptypes[0].system[s]].item;
				if (i > -1)
					starmap_additem(i, 0);
			}
			shiptypes[0].system[s] = itemtypes[it].index;
			shiptypes[0].sysdmg[s] = 0;
			starmap_removeitem(n);
		}
	}

	sort_shiptype_systems(0);
	starmap_advancedays(d);
//	shiptypes[0].speed = (shipsystems[shiptypes[0].thrust].par[0] * 32) / hulls[shiptypes[0].hull].mass;
}

void starmap_uninstallsystem(int32 n, int32 brk)
{
	int c, i;

	i = shipsystems[shiptypes[0].system[n]].item;
	if (i>-1)
	{
		starmap_additem(shipsystems[shiptypes[0].system[n]].item, brk);

		if (shipsystems[shiptypes[0].system[n]].type == sys_weapon)
		{
			shiptypes[0].system[n] = 0;
			shiptypes[0].sysdmg[n] = 0;
		}
		else
		{
			for (c = n; c < shiptypes[0].num_systems-1; c++)
			{
				shiptypes[0].system[c] = shiptypes[0].system[c+1];
				shiptypes[0].sysdmg[c] = shiptypes[0].sysdmg[c+1];
			}
			shiptypes[0].num_systems--;
			shiptypes[0].system[shiptypes[0].num_systems] = -1;
			shiptypes[0].sysdmg[shiptypes[0].num_systems] = 0;
		}
	}
	sort_shiptype_systems(0);
}

void starmap_destroysystem(int32 n)
{
	int c, i;

	i = shipsystems[shiptypes[0].system[n]].item;
	if (i>-1)
	{
		if (shipsystems[shiptypes[0].system[n]].type == sys_weapon)
		{
			shiptypes[0].system[n] = 0;
			shiptypes[0].sysdmg[n] = 0;
		}
		else
		{
			for (c = n; c < shiptypes[0].num_systems-1; c++)
			{
				shiptypes[0].system[c] = shiptypes[0].system[c+1];
				shiptypes[0].sysdmg[c] = shiptypes[0].sysdmg[c+1];
			}
			shiptypes[0].num_systems--;
			shiptypes[0].system[shiptypes[0].num_systems] = -1;
			shiptypes[0].sysdmg[shiptypes[0].num_systems] = 0;
		}
	}
	sort_shiptype_systems(0);
}

void starmap_additem(int32 it, int32 brk)
{
	player.items[player.num_items]=it;
	player.itemflags[player.num_items]=brk;

	player.num_items++;
}

void starmap_removeitem(int32 n)
{
	int c;

	for (c = n; c < player.num_items-1; c++)
	{
		player.items[c] = player.items[c+1];
		player.itemflags[c] = player.itemflags[c+1];
	}
	player.items[player.num_items] = -1;
	player.itemflags[player.num_items] = 0;
	player.num_items--;
}

int32 select_weaponpoint()
{
	int32 mc, c;
	int32 end = 0;
	int32 p = -1;
	int32 n;
	int32 bx = 240, by = 152;
	int32 mx, my;
	int32 upd = 1;
	int32 t;

	for (n = 0; n < hulls[shiptypes[0].hull].numh; n++)
	if (hulls[shiptypes[0].hull].hardpts[n].type == hdpWeapon &&
			shiptypes[0].sysdmg[n]==0 &&
			shipsystems[shiptypes[0].system[n]].item == -1)
	{
		p = n;
	}

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick() & 3;	
		c = ik_inkey();
		mx = ik_mouse_x - bx; my = ik_mouse_y - by;

		if (mc == 1)
		{
			upd = 1;
			if (my > 24 && my < 152)
			{
				//p = -1;
				for (n = 0; n < hulls[shiptypes[0].hull].numh; n++)
				if (hulls[shiptypes[0].hull].hardpts[n].type == hdpWeapon &&
						shiptypes[0].sysdmg[n]==0)
				{
					if (abs(16+hulls[shiptypes[0].hull].hardpts[n].x*2 - mx) < 6 &&
							abs(24+hulls[shiptypes[0].hull].hardpts[n].y*2 - my) < 6)
					{
						p = n;
						Play_SoundFX(WAV_INSTALL2, get_ik_timer(0));
					}
				}
			}
			else if (my > 152 && my < 168)
			{
				if (mx > 16 && mx < 64)
				{	end = 1; p = -1; Play_SoundFX(WAV_DECLINE, get_ik_timer(0)); }
				else if (mx > 96 && mx < 144)
				{ end = 1; Play_SoundFX(WAV_ACCEPT, get_ik_timer(0)); }
			}
		}

		if (c == 13 && p > -1)
		{	end = 1; Play_SoundFX(WAV_ACCEPT); }

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ upd = 1;	}

		if (upd)
		{
			upd=0;
			prep_screen();
			interface_drawborder(screen,
													 bx, by, bx+160, by+176,
													 1, STARMAP_INTERFACE_COLOR, textstring[STR_INV_POINT]);
			interface_drawbutton(screen, bx+16, by+176-24,48, STARMAP_INTERFACE_COLOR, textstring[STR_CANCEL]);
			interface_drawbutton(screen, bx+160-64, by+176-24,48, STARMAP_INTERFACE_COLOR, textstring[STR_OK]);

			ik_dsprite(screen, bx+16, by+24, hulls[shiptypes[0].hull].silu, 2+(STARMAP_INTERFACE_COLOR<<8));
			for (n = 0; n < hulls[shiptypes[0].hull].numh; n++)
			if (hulls[shiptypes[0].hull].hardpts[n].type == hdpWeapon)
			{
				ik_dsprite(screen, bx+8+hulls[shiptypes[0].hull].hardpts[n].x*2, 
													 by+16+hulls[shiptypes[0].hull].hardpts[n].y*2, 
													 spr_IFsystem->spr[1], 2+((shiptypes[0].sysdmg[n]==0)<<8));
				if (p==n)
				ik_dsprite(screen, bx+8+hulls[shiptypes[0].hull].hardpts[n].x*2, 
													 by+16+hulls[shiptypes[0].hull].hardpts[n].y*2, 
													 spr_IFsystem->spr[0], 0);
			}

			ik_blit();
		}
	}

	return p;
}

// klakar trade

void klakar_trade()
{
	int32 mc, c;
	int32 end = 0;
	int32 bx = 16, by = SM_INV_Y-56, h = 168;
	int32 plx = 0, klx = 212, iny = 36, inw = 180; // klx = 160; inw = 128
	int32 mx, my;
	int32 upd = 1;
	int32 t, y;
	int32 i, f;
	int32 clt;
	int32 sel1 = 0, sel2 = 0;
	int32 scr1=0, scr2=0;
	int32 co;
	int32 ploog[64];
	int32 num_ploog;
	t_ik_font *tradefont;
//	t_ik_sprite *bg;

	if (settings.opt_mucrontext & 2)
	{
		tradefont = font_6x8;
		klx = 212; inw = 180;
	}
	else
	{
		tradefont = font_4x8;
		klx = 160; inw = 128;
	}

	halfbritescreen();

	num_ploog=0;
	for (y = 0; y < player.num_items + shiptypes[0].num_systems; y++)
	{
		if (y < player.num_items)	// inventory
		{	
			if (!(itemtypes[player.items[y]].flag & 2) && !player.itemflags[y])
				ploog[num_ploog++]=y;
		}
		else		// system
		{	
			i = shipsystems[shiptypes[0].system[y-player.num_items]].item;
			if (i > -1 && !shiptypes[0].sysdmg[y-player.num_items] && !(itemtypes[i].flag & 2))
				ploog[num_ploog++]=y;
		}
	}

	if (!num_ploog)
		return;


	t = -1;
	for (c = 0; c < STARMAP_MAX_FLEETS; c++)
	if (sm_fleets[c].num_ships > 0)
		if (sm_fleets[c].race == race_klakar)
		{
			sm_fleets[c].system = player.system;
			t = c;
		}

	if (t==-1)
		return;


	Play_Sound(WAV_TRADE, 15, 1, 75);

	clt = 0;

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		t = get_ik_timer(0);
		mc = ik_mclick();	
		c = ik_inkey();
		mx = ik_mouse_x - bx; my = ik_mouse_y - by;

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ upd = 1;	}

		if (mc == 1)
		{
			if (my > iny+4 && my < iny+100) // select item or trade
			{
				c = (my - iny - 4) / 8;
				if (mx > klx+16 && mx < klx+inw+4 && c+scr1 < kla_numitems)
				{	
					y = sel1;
					sel1 = c + scr1; 
					Play_SoundFX(WAV_SELECT, t);
					if (y==sel1 && t-clt<20)	// double click for info
					{
						Play_SoundFX(WAV_INFO);
						interface_popup(font_6x8, bx+64, by+24, 192, 96, STARMAP_INTERFACE_COLOR, 0, 
														itemtypes[kla_items[sel1]].name, itemtypes[kla_items[sel1]].text);
						upd=1;
					}
				}
				if (mx > klx+inw+3 && mx < klx+inw+12 && kla_numitems > 12)
				{
					Play_SoundFX(WAV_SLIDER, t);
					if (my < iny+12)
						scr1 = MAX(0,scr1-1);
					else if (my > iny+92)
						scr1 = MIN(kla_numitems-12, scr1+1);
					else
						scr1 = MIN(((my - (iny+12))*(kla_numitems-12)+40) / 80, kla_numitems-12);
				}
				if (mx > plx+16 && mx < plx+inw+4 && c+scr2 < num_ploog)
				{	
					y = sel2;
					sel2 = c + scr2;
					Play_SoundFX(WAV_SELECT, t);
					if (y==sel2 && t-clt<20)	// double click for info
					{
						Play_SoundFX(WAV_INFO);
						if (ploog[sel2] < player.num_items) // inventory
							c = player.items[ploog[sel2]]; 
						else	// system
							c = shipsystems[shiptypes[0].system[ploog[sel2]-player.num_items]].item; 

						interface_popup(font_6x8, bx+64, by+24, 192, 96, STARMAP_INTERFACE_COLOR, 0, 
														itemtypes[c].name, itemtypes[c].text);
						upd=1;
					}
				}
				if (mx > plx+inw+3 && mx < plx+inw+12 && num_ploog > 12)
				{
					Play_SoundFX(WAV_SLIDER, t);
					if (my < iny+12)
						scr2 = MAX(0,scr2-1);
					else if (my > iny+92)
						scr2 = MIN(num_ploog-12, scr2+1);
					else
						scr2 = MIN(((my - (iny+12))*(num_ploog-12)+40) / 80, num_ploog-12);
				}

				if (mx > klx-12 && mx < klx+12) // trade (148 - 172)
				{
					Play_SoundFX(WAV_CASH, t);
					f = kla_items[sel1]; c = -1;
					if (ploog[sel2] < player.num_items) // inventory
					{
						kla_items[sel1] = player.items[ploog[sel2]]; 
						player.items[ploog[sel2]] = f;
						c = ploog[sel2];
						if (itemtypes[f].type != item_weapon && itemtypes[f].type != item_system)
							c = -1;
					}
					else	// system
					{
						t = ploog[sel2]-player.num_items;
						i = shipsystems[shiptypes[0].system[t]].item;
						kla_items[sel1] = shipsystems[shiptypes[0].system[t]].item;
						if (itemtypes[f].type == itemtypes[i].type)
						{
							if (itemtypes[f].type == item_weapon)	// swap weapons
							{
								shiptypes[0].system[t] = itemtypes[f].index;
								f = -1;
							}
							else	// other systems
							{
								if (shipsystems[itemtypes[i].index].type == shipsystems[itemtypes[f].index].type || shipsystems[itemtypes[f].index].type == sys_misc)
								{
									starmap_destroysystem(t);
									starmap_additem(f, 0);
									starmap_installitem(player.num_items-1);									
									f = -1;
								}
							}
						}
						if (f > -1)
						{
							starmap_destroysystem(t);
							starmap_additem(f, 0);
							c = player.num_items-1;
						}
					}
					if (itemtypes[f].type != item_weapon && itemtypes[f].type != item_system)
						c = -1;
					if (c > -1)
					{
						for (i = 0; i < shiptypes[0].num_systems; i++)
							if (c > -1)
								if (shipsystems[shiptypes[0].system[i]].type == shipsystems[itemtypes[player.items[c]].index].type 
										&& shipsystems[itemtypes[player.items[c]].index].type != sys_misc  
										&& itemtypes[player.items[c]].type != item_weapon)
									c = -1; 
						if (c > -1) // && itemtypes[player.items[c]].type != item_weapon)
						{
							if (itemtypes[player.items[c]].type == item_weapon)
							{
								for (i = 0; i < shiptypes[0].num_systems; i++)
									if (c > -1 && shipsystems[shiptypes[0].system[i]].type == sys_weapon && shipsystems[shiptypes[0].system[i]].item == -1)	// empty slot
									{
										shiptypes[0].system[i] = itemtypes[player.items[c]].index;
										starmap_removeitem(c);
										c = -1;
									}
							}
							else
								starmap_installitem(c);
						}
					}
					
					sort_shiptype_systems(0);
					upd = 1;
				}

				upd = 1;
			}
			if (my > h-24 && my < h-8) // button row
			{
				if (mx > klx+16 && mx < klx+48) // info
				{
					Play_SoundFX(WAV_INFO);
					interface_popup(font_6x8, bx+64, by+24, 192, 96, STARMAP_INTERFACE_COLOR, 0, 
													itemtypes[kla_items[sel1]].name, itemtypes[kla_items[sel1]].text);
					upd=1;
				}
				if (mx > plx+16 && mx < plx+48) // info
				{
					Play_SoundFX(WAV_INFO);
					if (ploog[sel2] < player.num_items) // inventory
						c = player.items[ploog[sel2]]; 
					else	// system
						c = shipsystems[shiptypes[0].system[ploog[sel2]-player.num_items]].item; 

					interface_popup(font_6x8, bx+64, by+24, 192, 96, STARMAP_INTERFACE_COLOR, 0, 
													itemtypes[c].name, itemtypes[c].text);
					upd=1;
				}
				if (mx > klx*2-80 && mx < klx*2-16) // done  (240 - 304)
				{	end = 1; Play_SoundFX(WAV_DOT, get_ik_timer(0)); }
			}
			clt = t;
		}

		if (upd)
		{
			upd = 0;

			num_ploog=0;
			for (y = 0; y < player.num_items + shiptypes[0].num_systems; y++)
			{
				if (y < player.num_items)	// inventory
				{	
					if (!(itemtypes[player.items[y]].flag & 2) && !player.itemflags[y])
						ploog[num_ploog++]=y;
				}
				else		// system
				{	
					if (shipsystems[shiptypes[0].system[y-player.num_items]].item > -1 && !shiptypes[0].sysdmg[y-player.num_items])
						ploog[num_ploog++]=y;
				}
			}

			prep_screen();

			interface_drawborder(screen, bx, by, bx + klx*2, by+h,
													 1, STARMAP_INTERFACE_COLOR, textstring[STR_TRADE_TITLE]);

			// player side
			ik_print(screen, font_6x8, bx+plx+16, by+24, STARMAP_INTERFACE_COLOR, "%s", player.shipname);
			interface_thinborder(screen, bx+plx+16, by+iny, bx+plx+inw+16, by+iny+104, STARMAP_INTERFACE_COLOR, STARMAP_INTERFACE_COLOR*16+1);	// +144

			for (c = 0; c < num_ploog; c++)
			{
				if (c >= scr2 && c < scr2 + 12)
				{
					if (sel2==c)	// highlight
						ik_drawbox(screen, bx+plx+16, by+iny+3+(c-scr2)*8, bx+plx+inw+15, by+iny+11+(c-scr2)*8, 3); //STARMAP_INTERFACE_COLOR*16+4);

					if (ploog[c] < player.num_items)
						i = player.items[ploog[c]];
					else
					{
						i = shipsystems[shiptypes[0].system[ploog[c]-player.num_items]].item;
						ik_dsprite(screen, bx+plx+15, by+iny-1+(c-scr2)*8, spr_IFsystem->spr[9], 2+(STARMAP_INTERFACE_COLOR<<8));
					}
					co = item_colorcode(i);
					ik_print(screen, tradefont, bx+plx+28, by+iny+4+(c-scr2)*8, co, itemtypes[i].name);
				}
			}
			if (num_ploog > 12)
			{
				ik_dsprite(screen, bx+plx+inw+4, by+iny+4, spr_IFarrows->spr[5], 2+(STARMAP_INTERFACE_COLOR<<8));
				ik_dsprite(screen, bx+plx+inw+4, by+iny+92, spr_IFarrows->spr[4], 2+(STARMAP_INTERFACE_COLOR<<8));
				interface_drawslider(screen, bx+plx+inw+4, by+iny+12, 1, 80, num_ploog-12, scr2, STARMAP_INTERFACE_COLOR);
			}

			interface_thinborder(screen, bx+plx+16, by+iny, bx+plx+inw+16, by+iny+104, STARMAP_INTERFACE_COLOR);

			// klakar side
			ik_print(screen, font_6x8, bx+klx+16, by+24, STARMAP_INTERFACE_COLOR, textstring[STR_TRADE_EMPORIUM]);
			interface_thinborder(screen, bx+klx+16, by+iny, bx+klx+inw+16, by+iny+104, STARMAP_INTERFACE_COLOR, STARMAP_INTERFACE_COLOR*16+1);
			for (c = 0; c < kla_numitems; c++)
			if (c >= scr1 && c < scr1 + 12)
			{
				if (sel1==c)	// highlight
					ik_drawbox(screen, bx+klx+16, by+iny+3+(c-scr1)*8, bx+klx+inw+15, by+iny+12+(c-scr1)*8, 3); //STARMAP_INTERFACE_COLOR*16+4);

				co = item_colorcode(kla_items[c]);
				ik_print(screen, tradefont, bx+klx+20, by+iny+4+(c-scr1)*8, co, itemtypes[kla_items[c]].name);
			}
			if (kla_numitems > 12)
			{
				ik_dsprite(screen, bx+klx+inw+4, by+iny+4, spr_IFarrows->spr[5], 2+(STARMAP_INTERFACE_COLOR<<8));
				ik_dsprite(screen, bx+klx+inw+4, by+iny+92, spr_IFarrows->spr[4], 2+(STARMAP_INTERFACE_COLOR<<8));
				interface_drawslider(screen, bx+klx+inw+4, by + iny+12, 1, 80, kla_numitems-12, scr1, STARMAP_INTERFACE_COLOR);
			}
			interface_thinborder(screen, bx+klx+16, by+iny, bx+klx+inw+16, by+iny+104, STARMAP_INTERFACE_COLOR);

			// trade and info buttons
			ik_dsprite(screen, bx+klx-12, by+iny+20, spr_IFbutton->spr[19], 2+(STARMAP_INTERFACE_COLOR<<8));
			ik_dsprite(screen, bx+16, by+h-24, spr_IFbutton->spr[11], 2+(STARMAP_INTERFACE_COLOR<<8));
			ik_dsprite(screen, bx+klx+16, by+h-24, spr_IFbutton->spr[11], 2+(STARMAP_INTERFACE_COLOR<<8));
			interface_drawbutton(screen, bx+klx*2-80, by+h-24, 64, STARMAP_INTERFACE_COLOR, textstring[STR_DONE]);

			ik_blit();
			if (settings.random_names & 4)
			{
				interface_tutorial(tut_trading);
			}

		}
	}

	Stop_Sound(15);

	must_quit = 0;

	reshalfbritescreen();
}


// mercenary payments

int32 pay_item(char *title, char *text, int r, char klak)
{
	int32 c, mc;
	int32 end = 0;
	int32 it = -1;
	int32 co, i;
	int32 bx = 240, by = 128, h = 224;
	int32 scr, sel;
	int32 upd = 1;
	int32 y, y1;
	int32 ploog[64];
	int32 num_ploog=0;
	int32 ti, clt, ot;

	for (y = 0; y < player.num_items + shiptypes[0].num_systems; y++)
	{
		if (y < player.num_items)	// inventory
		{	
			if (!(itemtypes[player.items[y]].flag & 2) && (itemtypes[player.items[y]].type != item_lifeform || klak == 1))
				ploog[num_ploog++]=y;
		}
		else		// system
		{	
			if (shipsystems[shiptypes[0].system[y-player.num_items]].item > -1 && shiptypes[0].sysdmg[y-player.num_items]==0)
				ploog[num_ploog++]=y;
		}
	}

	if (!num_ploog)
		return -1;

	halfbritescreen();

//	Stop_All_Sounds();
	Play_Sound(WAV_PAYMERC, 15, 1);

	while (!end && !must_quit)
	{
		sel = -1; scr = 0;
		clt = 0;

		while (!end && !must_quit)
		{
			ik_eventhandler();
//			t = get_ik_timer(0);
			mc = ik_mclick();	

			if (must_quit)
			{
				end = 1; it = -1;
				must_quit = 0;
			}

			ot = ti; ti = get_ik_timer(2);
			if (ti != ot)
			{ upd = 1;	}

			if (mc == 1)
			{
				if (ik_mouse_y > by+96 && ik_mouse_y < by+192)
				{
					if (ik_mouse_x > bx+16 && ik_mouse_x < bx + 180)
					{
						y = sel;
						sel = scr + (ik_mouse_y - (by+96))/8;
						if (sel > num_ploog || sel < 0)
						{	
							sel = -1; 
							Play_SoundFX(WAV_DESELECT, get_ik_timer(0));
						}
						else
							Play_SoundFX(WAV_SELECT, get_ik_timer(0));
						upd = 1;
						if (y==sel && ti-clt<20) // doubleclick for info
						{
							if (ploog[sel] < player.num_items)
								y = player.items[ploog[sel]];
							else
								y = shipsystems[shiptypes[0].system[ploog[sel]-player.num_items]].item;

							Play_SoundFX(WAV_INFO);
							interface_popup(font_6x8, bx+96, by+24, 192, 96, STARMAP_INTERFACE_COLOR, 0, 
															itemtypes[y].name, itemtypes[y].text);
							upd=1;
						}
					}
					else if (ik_mouse_x > bx + 180 && ik_mouse_x < bx+188 && num_ploog>12)
					{
						if (ik_mouse_y < by + 104)
							scr = MAX(0,scr-1);
						else if (ik_mouse_y > by + 184)
							scr = MIN(num_ploog-12, scr+1);
						else
							scr = MIN(((ik_mouse_y - (by+104))*(num_ploog-12)+40) / 80, num_ploog-12);
						Play_SoundFX(WAV_SLIDER, get_ik_timer(0));
						upd = 1;
					}
				}
				if (ik_mouse_y > by+h-24 && ik_mouse_y < by+h-8)
				{	
					if (ik_mouse_x > bx+16 && ik_mouse_x < bx+80)
					{	
						end = 1; it = -1; 
						Play_SoundFX(WAV_DECLINE, get_ik_timer(0));
					}
					else if (ik_mouse_x > bx+128 && ik_mouse_x < bx+192 && sel > -1)
					{	
						end = 1; 
						sel = ploog[sel];
						if (sel == -1)
							it = -1;
						else if (sel < player.num_items)
							it = player.items[sel];
						else
							it = shipsystems[shiptypes[0].system[sel-player.num_items]].item;
						Play_SoundFX(WAV_ACCEPT, get_ik_timer(0));
					}
				}
				clt=ti;
			}

			if (upd)
			{
				upd = 0;

				prep_screen();
				interface_drawborder(screen, bx, by, bx+208, by+h, 1, STARMAP_INTERFACE_COLOR, title);

				interface_textbox(screen, font_4x8,
													bx+84, by+24, 108, 64, 0,
													text);

				ik_dsprite(screen, bx+16, by+24, spr_SMraces->spr[r], 0);
				ik_dsprite(screen, bx+16, by+24, spr_IFborder->spr[IF_BORDER_PORTRAIT], 2+(STARMAP_INTERFACE_COLOR<<8));

				interface_drawbutton(screen, bx+16, by+h-24, 64, STARMAP_INTERFACE_COLOR, textstring[STR_DECLINE]);
				interface_drawbutton(screen, bx+128, by+h-24, 64, STARMAP_INTERFACE_COLOR, textstring[STR_ACCEPT]);

				interface_thinborder(screen, bx+16, by+92, bx+192, by+196, STARMAP_INTERFACE_COLOR, STARMAP_INTERFACE_COLOR*16+1);

				for (c = 0; c < num_ploog; c++)
				{
					y1 = c-scr;
					y = ploog[c];
					if (y1 >= 0 && y1 < 12)
					{
						if (c == sel)
							ik_drawbox(screen, bx+16, by+95+(c-scr)*8, bx+191, by+103+(c-scr)*8, 3); //STARMAP_INTERFACE_COLOR*16+4);

						if (y < player.num_items)	// inventory
							i = player.items[y];
						else		// system
						{
							i = shipsystems[shiptypes[0].system[y-player.num_items]].item;
							ik_dsprite(screen, bx+15, by+91+(c-scr)*8, spr_IFsystem->spr[9], 2+(STARMAP_INTERFACE_COLOR<<8));
						}
						co = item_colorcode(i);
						ik_print(screen, font_6x8, bx + 28, by + 96 + y1 * 8, co, itemtypes[i].name);
					}
				}
				if (num_ploog > 12)
				{
					ik_dsprite(screen, bx+180, by+96, spr_IFarrows->spr[5], 2+(STARMAP_INTERFACE_COLOR<<8));
					ik_dsprite(screen, bx+180, by+184, spr_IFarrows->spr[4], 2+(STARMAP_INTERFACE_COLOR<<8));
					interface_drawslider(screen, bx + 180, by + 104, 1, 80, num_ploog-12, scr, STARMAP_INTERFACE_COLOR);
				}
				interface_thinborder(screen, bx+16, by+92, bx+192, by+196, STARMAP_INTERFACE_COLOR);

				ik_blit();
			}
		}

		if (!klak && it>-1)	// mercenary hire
		{
			ik_print_log("try to give %s to %s\n", itemtypes[it].name, shiptypes[player.ships[player.num_ships-1]].name);
			it = ally_install(player.num_ships-1, it, 1);

			if (it == -1)
			{	end = 0; ik_print_log("didn't accept\n"); }
			else
				ik_print_log("accepted %s - ", itemtypes[it].name);
		}

		if (it>-1)
		{
			end = 1;
			if (sel < player.num_items)
			{
				ik_print_log("removed item %s\n", itemtypes[player.items[sel]].name);
				starmap_removeitem(sel);
			}
			else
			{
				ik_print_log("removed system %s\n", shipsystems[shiptypes[0].system[sel-player.num_items]].name);
				starmap_destroysystem(sel - player.num_items);
			}
		}

	}

	Stop_Sound(15);

	reshalfbritescreen();

	return it;
}

int32 ally_install(int32 s, int32 it, int32 pay)
{
	int32 r = -1;
	int32 sz = 1;
	int32 sys = -1;
	int32 isys, lc;
	int32 st = player.ships[s];
	int32 c;
	int32 m = 0;

	char texty[256];
	char sname[64];

	if (hulls[shiptypes[st].hull].size >= 32)
		sz = 2;

	if (itemtypes[it].type == item_system || itemtypes[it].type == item_weapon)
	{
		sys = itemtypes[it].index;
		ik_print_log("AI: Is system %s. ", shipsystems[sys].name);
	}

	if (!pay)
	{
		if (shiptypes[st].flag & 32)
			sprintf(sname, textstring[STR_ALLY_CAPT1], shiptypes[st].name);
		else
			sprintf(sname, textstring[STR_ALLY_CAPT2], shiptypes[st].name);
		sprintf(texty, textstring[STR_ALLY_CONFIRM], itemtypes[it].name, sname);
		if (interface_popup(font_6x8, 256, 192, 192, 0, STARMAP_INTERFACE_COLOR, 0, 
												textstring[STR_ALLY_CONFIRMT], texty, textstring[STR_YES], textstring[STR_NO]))
		{
			ik_print_log("AI: User cancelled.\n");
			return -1;
		}

		if (sys > -1)
			if (shipsystems[sys].type == sys_engine)
			{ 
				sys = -1; 
				ik_print_log("AI: Refused engine. ");
			}
	}

	if (sys == -1)	// accept non-systems as is
	{
		if (pay)
		{
			sprintf(texty, textstring[STR_MERC_THANKS2], shiptypes[st].name, itemtypes[it].name);
			interface_popup(font_6x8, 256, 192, 192, 0, STARMAP_INTERFACE_COLOR, 0, 
													textstring[STR_ALLY_TITLE], texty, textstring[STR_OK]);
			return it;
		}
		else	// if a gift, refuse if unusable
		{	
			r = -1;		
			ik_print_log("AI: Refused nonsystem. ");
		}
	}
	else if (shipsystems[sys].size <= sz)	// fits in the ship
	{
		// replace same type of system if better 
		ik_print_log("AI: %s fits on ship. ", shipsystems[sys].name);
		if (shipsystems[sys].type != sys_misc)
		{
			isys = -1; lc = -1;
			for (c = 0; c < shiptypes[st].num_systems; c++)
			if (shipsystems[shiptypes[st].system[c]].type == shipsystems[sys].type)
			{
				m = 1;	// matches current system type
				ik_print_log("AI: %s matches %s on ship. ", shipsystems[sys].name, shipsystems[shiptypes[st].system[c]].name);
				// check the cost for "betterness"
				if (itemtypes[shipsystems[shiptypes[st].system[c]].item].cost < itemtypes[it].cost)
				{
					if (lc == -1 || itemtypes[shipsystems[shiptypes[st].system[c]].item].cost < lc)
					{
						isys = c;
						lc = itemtypes[shipsystems[shiptypes[st].system[c]].item].cost;
					}
				}
			}	
			
			if (isys > -1)
			{
				ik_print_log("AI: %s replaces %s. ", shipsystems[itemtypes[it].index].name, shipsystems[shiptypes[st].system[isys]].name);
				shiptypes[st].system[isys] = itemtypes[it].index;
				r = it; 
			}
		}
		
		if (!m)	// didn't match the type of any current systems, so install it!
		{
			ik_print_log("AI: No match, install %s. ", shipsystems[itemtypes[it].index].name);
			shiptypes[st].system[shiptypes[st].num_systems++] = itemtypes[it].index;
			r = it;
		}
	
	}
	else
	{
		r = -1;
		ik_print_log("AI: can't install. ");
	}

	ik_print_log("AI: sort systems. ");
	sort_shiptype_systems(st);

	if (pay)
	{
		if (r == -1)	// didn't install
		{
			if (!m)
			{	
				sprintf(texty, textstring[STR_MERC_TOOBIG], shiptypes[st].name, itemtypes[it].name);
				ik_print_log("AI: too big. ");
			}
			else
			{
				sprintf(texty, textstring[STR_MERC_NOGOOD], shiptypes[st].name, itemtypes[it].name);
				ik_print_log("AI: not good enough. ");
			}
			r = interface_popup(font_6x8, 256, 192, 192, 0, STARMAP_INTERFACE_COLOR, 0, 
													textstring[STR_MERC_NOGOODT], texty, textstring[STR_YES], textstring[STR_NO]);

			if (r == 0)
			{
				r = it;
				sprintf(texty, textstring[STR_MERC_THANKS2], shiptypes[st].name, itemtypes[it].name);
				interface_popup(font_6x8, 256, 192, 192, 0, STARMAP_INTERFACE_COLOR, 0, 
														textstring[STR_ALLY_TITLE], texty, textstring[STR_OK]);
				ik_print_log("AI: take it anyway. ");
			}
			else
			{
				r = -1;
				ik_print_log("AI: don't take it. ");
			}
		}
		else
		{
			sprintf(texty, textstring[STR_MERC_THANKS], shiptypes[st].name);
			interface_popup(font_6x8, 256, 192, 192, 0, STARMAP_INTERFACE_COLOR, 0, 
													textstring[STR_ALLY_TITLE], texty, textstring[STR_OK]);
			ik_print_log("AI: thank you ");
		}
	}
	else	// gift
	{
		if (r == -1)
		{
			if (shiptypes[st].flag & 32)
				sprintf(sname, textstring[STR_ALLY_CAPT1], shiptypes[st].name);
			else
				sprintf(sname, textstring[STR_ALLY_CAPT2], shiptypes[st].name);
			sprintf(texty, textstring[STR_ALLY_REFUSE], sname);
			interface_popup(font_6x8, 256, 192, 192, 0, STARMAP_INTERFACE_COLOR, 0, 
											textstring[STR_ALLY_REFUSET], texty, textstring[STR_OK]);
			ik_print_log("AI: refused. ");
		}
		else
		{
			if (shiptypes[st].flag & 32)
				sprintf(sname, textstring[STR_ALLY_SHIP1], shiptypes[st].name);
			else
				sprintf(sname, textstring[STR_ALLY_SHIP2], shiptypes[st].name);
			sprintf(texty, textstring[STR_ALLY_INSTALL], itemtypes[r].name, sname);
			interface_popup(font_6x8, 256, 192, 192, 0, STARMAP_INTERFACE_COLOR, 0, 
											textstring[STR_ALLY_INSTALLT], texty, textstring[STR_OK]);
			ik_print_log("AI: thanks for the gift (%s). ", itemtypes[r].name);
		}
	}
	ik_print_log("\n");
		

	return r;
}

// use artifacts
#ifndef DEMO_VERSION

int32 use_vacuum_collapser(char *title)
{
	int32 mc, c;
	int32 end = 0;
	int32 bl = 0;
	int32 t;
	t_ik_sprite *bg;
	int32 bx = SM_MAP_X + 144, by = SM_MAP_Y + 200 + (1-2*(sm_stars[player.system].y < 0))*80;

	char texty[256];
	char nummy[16];
	int32 num = 0;

	sprintf(texty, textstring[STR_LVC_CONFIRM], sm_stars[player.system].starname);

	if (interface_popup(font_6x8, bx, by, 192, 80, STARMAP_INTERFACE_COLOR, 0, 
			title, texty, textstring[STR_YES], textstring[STR_NO]))
		return 0;	// cancel

	// set timer here

	prep_screen();

	bg = get_sprite(screen, bx, by, 192, 80);

	interface_drawborder(screen,
											 bx, by, bx+192, by+80,
											 1, STARMAP_INTERFACE_COLOR, title);

	sprintf(texty, textstring[STR_LVC_ASKWHEN]);
	interface_textbox(screen, font_6x8,
										bx+16, by+24, 160, 24, 0,
										texty);
	ik_print(screen, font_6x8, bx+16, by+48, 0, textstring[STR_LVC_DAYSTILL]);
	ik_blit();

	sprintf(nummy, "");
	ik_text_input(bx+176-24, by+48, 4, font_6x8, "", nummy, STARMAP_INTERFACE_COLOR*16+3, STARMAP_INTERFACE_COLOR);

	num = 0;

	if (!strlen(nummy))
		end = 1;
	for (c = 0; c < (int)strlen(nummy); c++)
	{
		if (nummy[c] >= '0' && nummy[c] <= '9')
			num = num*10 + nummy[c]-'0';
	}
	prep_screen();
	ik_drawbox(screen, bx+176-24, by+48, bx+176, by+55, STARMAP_INTERFACE_COLOR*16+3);
	ik_print(screen, font_6x8, bx+176-24, by+48, 3, "%4d", num);

	interface_drawbutton(screen, bx+192-64, by + 80 - 24, 48, STARMAP_INTERFACE_COLOR, textstring[STR_OK]);
	interface_drawbutton(screen, bx+16, by + 80 - 24, 48, STARMAP_INTERFACE_COLOR, textstring[STR_CANCEL]);

	ik_blit();

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick() & 3;	
		c = ik_inkey();

		if (c == 13)
			end = 2;

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ prep_screen(); ik_blit();	}

		if (mc == 1 && ik_mouse_y > by+48 && ik_mouse_y < by+56)
		{
			if (ik_mouse_x > bx+176-24 && ik_mouse_x < bx+176)
			{
				sprintf(nummy, "%d", num);
				ik_text_input(bx+176-24, by+48, 4, font_6x8, "", nummy, STARMAP_INTERFACE_COLOR*16+3, STARMAP_INTERFACE_COLOR);

				num = 0;
				if (!strlen(nummy))
					end = 1;
				for (c = 0; c < (int)strlen(nummy); c++)
				{
					if (nummy[c] >= '0' && nummy[c] <= '9')
						num = num*10 + nummy[c]-'0';
				}
				prep_screen();
				ik_drawbox(screen, bx+176-24, by+48, bx+176, by+55, STARMAP_INTERFACE_COLOR*16+3);
				ik_print(screen, font_6x8, bx+176-24, by+48, 3, "%4d", num);
				ik_blit();
			}
		}

		if (mc == 1 && ik_mouse_y > by+80-24 && ik_mouse_y < by+80-8)
		{
			if (ik_mouse_x > bx+192-64 && ik_mouse_x < bx+192-16)
				end = 2;
			else if (ik_mouse_x > bx+16 && ik_mouse_x < bx+64)
				end = 1;
		}
	}

	prep_screen();
	ik_dsprite(screen, bx, by, bg, 4);
	ik_blit();
	free_sprite(bg);

	if (must_quit)
	{	must_quit = 0; end = 1; }

	if (end == 2)	// use vacuum collapser
	{
		Play_SoundFX(WAV_YES, get_ik_timer(0));

		sm_stars[player.system].novadate = player.stardate + num;
		sm_stars[player.system].novatype = 2;
	}
	else if (end == 1)
		Play_SoundFX(WAV_NO, get_ik_timer(0));

	return end-1;
}

void vacuum_collapse(int st)
{
	int32 end = 0;
	int32 cx, cy;
	int32 x,y;
	int32 x1,y1,d,s;
	int32 tx,ty;
	int32 r, c, t, t0, ts;
	int32 f;
	//int32 ti2;
	int32 sz;
	uint8 *data;

	cx = sm_stars[st].x; cy = sm_stars[st].y;

	Play_SoundFX(WAV_COLLAPSER);

	ts = t0 = t = get_ik_timer(2);

	sm_stars[st].novatype = 2;
	sm_stars[st].novatime = t+25;

	f = 0;
	while (!end && !must_quit)
	{
		ik_eventhandler();

		t0 = t;
		t = get_ik_timer(2);

		if (t > ts + 25 && sm_stars[st].color > -2)
			sm_stars[st].color = -3;

		cx = sm_stars[st].x; cy = sm_stars[st].y;

		if (t > t0)
		{

			if (t > sm_stars[st].novatime + 150)
				end = 1;

			if (t >= sm_stars[st].novatime && t < sm_stars[st].novatime + 50)
			{
				sz = (t - sm_stars[st].novatime)*2;

				if (player.num_ships > 0)
				{
					r = get_distance(cx - player.x, cy - player.y);
					if (r < sz || (player.enroute && sm_stars[player.target].color < 0))	// kill player
					{
						player.num_ships = 0;
						player.death = 3;
						player.deatht = t;
					}
				}

				for (c = 0; c < num_holes; c++)
				{
					r = get_distance(cx - sm_holes[c].x, cy - sm_holes[c].y);
					if (r < sz)	// five light year radius of destruction
						sm_holes[c].size = 0;
				}
				for (c = 0; c < num_stars; c++)
				if (c != st && sm_stars[c].color > -2)
				{
					r = get_distance(cx - sm_stars[c].x, cy - sm_stars[c].y);
					if (r < sz)	// five light year radius of destruction
					{
						
						Play_SoundFX(WAV_EXPLO2, t);
						sm_stars[c].novatype = 3;
						sm_stars[c].novadate = player.stardate - 1;
						sm_stars[c].novatime = t;
						sm_stars[c].color = -3;
						for (r = 0; r < STARMAP_MAX_FLEETS; r++)
						if (sm_fleets[c].num_ships > 0 && sm_fleets[c].enroute == 0 && sm_fleets[c].system == c)
						{
							sm_fleets[r].num_ships = 0;
						}
					}
				}

				for (c = 0; c < STARMAP_MAX_FLEETS; c++)
				if (sm_fleets[c].num_ships > 0)
				{
					if (sm_fleets[c].enroute)
					{
						if (sm_fleets[c].distance > 0)
						{
							x = sm_stars[sm_fleets[c].system].x + ((sm_stars[sm_fleets[c].target].x-sm_stars[sm_fleets[c].system].x)*sm_fleets[c].enroute)/sm_fleets[c].distance;
							y = sm_stars[sm_fleets[c].system].y + ((sm_stars[sm_fleets[c].target].y-sm_stars[sm_fleets[c].system].y)*sm_fleets[c].enroute)/sm_fleets[c].distance;
						}
						else
						{
							x = sm_stars[sm_fleets[c].target].x;
							y = sm_stars[sm_fleets[c].target].y;
						}
					}
					else
					{
						x = sm_stars[sm_fleets[c].system].x;
						y = sm_stars[sm_fleets[c].system].y;
					}

					r = get_distance ( cx - x, cy - y );
					if (r < sz)
					{
						if (sm_fleets[c].explored < 1)
							sm_fleets[c].explored = 1;
						sm_fleets[c].num_ships = 0;
						sm_fleets[c].blowtime = t;
						Play_SoundFX(WAV_EXPLO2, t);
					}

				}
			
			}

			if (t > sm_stars[st].novatime && t < sm_stars[st].novatime + 50)
			{
				if (!f)
				{
					cx = 240 + sm_stars[st].x; cy = 240 - sm_stars[st].y;
					s = ((t - sm_stars[st].novatime)<<8)/50;
					d = (128<<8)/s;
					ty = 0;
					for (y1 = cy - s/2; y1 < cy + s/2; y1++)
					{
						tx = 0;
						if (y1>=0 && y1 < 480)
						for (x1 = cx - s/2; x1 < cx + s/2; x1++)
						{
							if (x1>=0 && x1 < 480)
							{
								data = spr_SMnebula->spr[8]->data;
								x = ((ty>>8)<<7) + (tx>>8);

//								r = ((data[x] * (256-(tx&255)) + data[x+1] * (tx&255)) * (256-(ty&255)) + 
//										(data[x+128] * (256-(tx&255)) + data[x+129] * (tx&255)) * (ty&255)) >> 16;
								r = data[x];
								t = sm_nebulamap[(y1<<9)+x1];
								if (r < 15)
									sm_nebulamap[(y1<<9)+x1] = (t * r) / 15;
							}
							tx+=d;
						}
						ty+=d;
					}
				}
				else
				{
					starmap_createnebulagfx();
				}
			}

			prep_screen();
			starmap_display(get_ik_timer(0));
			f = 1-f;

//			ik_print(screen, font_6x8, SM_MAP_X, SM_MAP_Y, 3, "%d (%d)", t - sm_stars[st].novatime, t);
			ik_blit();
		}
	}

	sm_stars[st].color = -2;
	sm_stars[st].novadate = 0;
	sm_stars[st].novatype = 0;

	if (sm_stars[homesystem].color == -2 && player.death != 3)
		player.death = 7;

	cx = 240 + sm_stars[st].x; cy = 240 - sm_stars[st].y;
	for (y1 = cy - 128; y1 < cy + 127; y1++)
		if (y1>=0 && y1 < 480)
		for (x1 = cx - 128; x1 < cx + 127; x1++)
			if (x1>=0 && x1 < 480)
			{
				x = (x1+128-cx); y = (y1+128-cy);
				data = spr_SMnebula->spr[8]->data;
				r = ((data[(y>>1)*128+(x>>1)] +
							data[(y>>1)*128+(x>>1)+1]*(x&1) +
							data[(y>>1)*128+(x>>1)+128]*(y&1) +
							data[(y>>1)*128+(x>>1)+129]*(x&1)*(y&1)) * 4) / (1+(x&1)+(y&1)+(x&1)*(y&1));
				t = sm_nebulamap[(y1<<9)+x1];
				if (r < 60)
					sm_nebulamap[(y1<<9)+x1] = (t * r) / (15 * 4);
			}

	starmap_createnebulagfx();

	must_quit = 0;
}
#endif

int32 probe_fleet_encounter(int32 flt)
{
	int32 mc, c;
	int32 end = 0;
	int32 bx = 240, by = 152;
	int32 mx, my;
	int32 r, t, t0;
	int32 l;
	int32	upd = 1;
	int32 sx[8], sy[8];
	int32 survive;
	char texty[256];

	halfbritescreen();

	r = sm_fleets[flt].race;

	if (races[r].met)
		sm_fleets[flt].explored=2;
	else
		sm_fleets[flt].explored=1;

	Stop_All_Sounds();

	Play_SoundFX(WAV_PROBE_DEST, 0);

	for (c = 0; c < sm_fleets[flt].num_ships; c++)
	{
		mc = hulls[shiptypes[sm_fleets[flt].ships[c]].hull].size;
		sx[c] = bx + 70 + c * 16 - (sm_fleets[flt].num_ships-1) * 8 + rand()%12;
		sy[c] =	by + 56 + rand()%16;
	}

	if (r == race_klakar || races[r].met==2)	// klakar or friend-muktians
		survive = 1;
	else if (rand()&1)
		survive = 1;
	else
		survive = 0;

	Play_Sound(WAV_RADAR, 15, 1);

	t = get_ik_timer(0);
	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	
		t0 = t; t = get_ik_timer(0);
		mx = ik_mouse_x - bx; my = ik_mouse_y - by;
		if (mc == 1 && mx > 112 && mx < 144 && my > 184 && my < 200)
		{	end = 1; Play_SoundFX(WAV_DOT, get_ik_timer(0)); }

		if (t > t0 || upd == 1)
		{
			upd = 0;
			prep_screen();
			interface_drawborder(screen,
													 bx, by, bx+160, by+208,
													 1, STARMAP_INTERFACE_COLOR, textstring[STR_PROBE_TITLE]);
			if (races[r].met)
				ik_print(screen, font_6x8, bx+16, by+24, 3, textstring[STR_SCANNER_RACE], races[r].name);
			else
				ik_print(screen, font_6x8, bx+16, by+24, 3, textstring[STR_SCANNER_NORACE]);

			if (survive)
			{
				if (sm_fleets[flt].num_ships>1)
					sprintf(texty, textstring[STR_PROBE_FLEET1], sm_fleets[flt].num_ships);
				else
					sprintf(texty, textstring[STR_PROBE_FLEET2]);
			}
			else
			{
				sprintf(texty, textstring[STR_PROBE_FLEET3]);
			}
			interface_textbox(screen, font_4x8, bx+16, by+160, 128, 24, 0, 
												texty);

			ik_dsprite(screen, bx+16, by+32, spr_IFborder->spr[19], 2+(4<<8));
			l = 10 + 5 * ((t%50)>40);
			for (c = 0; c < sm_fleets[flt].num_ships; c++)
			{
				mc = hulls[shiptypes[sm_fleets[flt].ships[c]].hull].size;
				ik_drsprite(screen, sx[c], sy[c], 0, 8,
									 spr_IFarrows->spr[10-(mc<32)+(mc>80)], 5+(l<<8));
			}
			l = ((t&63)*3 - 95 + 1024) & 1023;
			ik_dspriteline(screen, bx + 80, by + 152, 
										bx + 80 + ((sin1k[l] * 112) >> 16), 
										by + 152 - ((cos1k[l] * 112) >> 16), 
										8, (t&31), 18, spr_weapons->spr[2], 5+(10<<8));
			interface_drawbutton(screen, bx+112, by+184, 32, STARMAP_INTERFACE_COLOR, textstring[STR_OK]);
			ik_blit();
		}

	}

	Stop_Sound(15);
	must_quit = 0;
	reshalfbritescreen();

	return survive;
}

void probe_exploreplanet(int32 probe)
{
	int32 mc, c;
	int32 h;
	int32 end = 0;
	int32 it = -1;
	int32 bx = 224, by = 112;
	int32 tof = 0;
	int32 t;
	char name[32];
	char texty[256];

	if (sm_stars[player.target].explored)
		return;

//	Stop_All_Sounds();

	if (!probe)	// regular stellar probe
	{
		for (c = 0; c < STARMAP_MAX_FLEETS; c++)
		{
			if (sm_fleets[c].num_ships>0 && sm_fleets[c].system == player.target)
				it=0;
		}
		if (it)
			Play_SoundFX(WAV_PROBE_DEST, 0);
	}
	else
	{
		tof = STR_ANALYZER_MISCDATA1 - STR_PROBE_MISCDATA1;
		Play_Sound(WAV_SCANNER, 15, 1);
	}

	it = -1;

	sm_stars[player.target].explored = 1;
	c = sm_stars[player.target].card; 

	h = 192;

	prep_screen();
	halfbritescreen();
	if (!probe)
	interface_drawborder(screen,
											 bx, by, bx+192, by+h,
											 1, STARMAP_INTERFACE_COLOR, textstring[STR_PROBE_TITLE]);
	else
	interface_drawborder(screen,
											 bx, by, bx+192, by+h,
											 1, STARMAP_INTERFACE_COLOR, textstring[STR_PROBE_TITLE2]);

	ik_print(screen, font_6x8, bx+16, by+24, 3, sm_stars[player.target].planetname);
	ik_print(screen, font_4x8, bx+176-strlen(textstring[STR_CARD_RENAME])*4, by+24, 3, textstring[STR_CARD_RENAME]);
	ik_dsprite(screen, bx+16, by+36, spr_SMplanet2->spr[sm_stars[player.target].planetgfx], 0);
	ik_dsprite(screen, bx+16, by+36, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
	interface_textbox(screen, font_6x8,
										bx+84, by+36, 96, 104, 0,
										platypes[sm_stars[player.target].planet].text);

	sprintf(name, textstring[STR_PROBE_MISCDATA]);
	sprintf(texty, textstring[STR_PROBE_MISCDATA1+tof]);
	if (ecards[c].type == card_event)
	{	
#ifndef DEMO_VERSION
		if (!strcmp(ecards[c].name, textstring[STR_EVENT_FLARE]) || !strcmp(ecards[c].name, textstring[STR_EVENT_NOVA]))
		{
			sprintf(texty, textstring[STR_PROBE_MISCDATA2+tof], sm_stars[player.target].starname, player.shipname);
		}
#else
		if (!strcmp(ecards[c].name, textstring[STR_EVENT_FLARE]))
		{
			sprintf(texty, textstring[STR_PROBE_MISCDATA2+tof], sm_stars[player.target].starname, player.shipname);
		}
#endif
	}
	else if (ecards[c].type == card_ally)
	{
		it = shiptypes[ecards[c].parm].race; 
		if (it == race_none)
			sprintf(texty, textstring[STR_PROBE_MISCDATA3+tof]);
#ifndef DEMO_VERSION
		if (it == race_muktian)
			sprintf(texty, textstring[STR_PROBE_MISCDATA4+tof]);
#endif
	}
	else if ((ecards[c].type == card_item) || (ecards[c].type == card_rareitem) || (ecards[c].type == card_lifeform))
	{
		it = ecards[c].parm;
		if (ecards[c].type == card_lifeform)
			sprintf(texty, textstring[STR_PROBE_MISCDATA5+tof]);

		if (itemtypes[it].type == item_treasure)
			sprintf(texty, textstring[STR_PROBE_MISCDATA6+tof]);
	}

	ik_print(screen, font_6x8, bx+96-strlen(name)*3, by+108, STARMAP_INTERFACE_COLOR, name);
	interface_textbox(screen, font_6x8,
										bx+16, by+120, 160, 48, 0,
										texty); 

	interface_drawbutton(screen, bx+144, by+h-24, 32, STARMAP_INTERFACE_COLOR, textstring[STR_OK]);

	ik_blit();

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	
		c = ik_inkey();

		if (mc == 1)
		{
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
		}

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ prep_screen(); ik_blit();	}
	}

	ik_print_log("Launched the Stellar Probe to the %s system, discovered a %s planet and named it %s.\n", 
								sm_stars[player.target].starname,
								platypes[sm_stars[player.target].planet].name,
								sm_stars[player.target].planetname);

	must_quit = 0;
	reshalfbritescreen();
	Stop_Sound(15);
}

int32 stellar_probe(char *title)
{
	char texty[256];	
	int32 p;
	int32 c;

	if (player.target	== -1 || sm_stars[player.target].explored)
	{	// no target or bad target - don't launch
		interface_popup(font_6x8, SM_INV_X + 32*(SM_INV_X==0) - 64*(SM_INV_X>0), SM_INV_Y+40, 192, 80, STARMAP_INTERFACE_COLOR, 0, 
										title, textstring[STR_PROBE_DIALOG1], textstring[STR_OK]);
		return 0;
	}

	sprintf(texty, textstring[STR_PROBE_DIALOG2], sm_stars[player.target].starname);
	if (!interface_popup(font_6x8, SM_INV_X + 32*(SM_INV_X==0) - 64*(SM_INV_X>0), SM_INV_Y+40, 192, 80, STARMAP_INTERFACE_COLOR, 0, 
										title, texty, textstring[STR_YES], textstring[STR_NO]))
	{
		Stop_All_Sounds();
		Play_SoundFX(WAV_PROBE_LAUNCH, 0);
		start_ik_timer(3, 20);
		while (!must_quit && get_ik_timer(3)<50)
			ik_eventhandler();
		p = 2;
		for (c = 0; c < STARMAP_MAX_FLEETS; c++)
		{
			if (sm_fleets[c].num_ships>0 && sm_fleets[c].system == player.target)	// find a fleet
			{
				p = probe_fleet_encounter(c);
			}
		}
		if (p)
			probe_exploreplanet(0);
		return 1;
	}	
	must_quit = 0;

	return 0;
}

#ifndef DEMO_VERSION
void eledras_mirror(char *title)
{
	int32 c;
	int32 f;
	int32 t;

	f = -1;
	for (c = 0; c < STARMAP_MAX_FLEETS; c++)
	{
		// find a fleet
		if (sm_fleets[c].explored>0 && sm_fleets[c].num_ships>0 && sm_fleets[c].system == player.target && sm_fleets[c].enroute==0)
		{
			f = c;
		}
	}


	if (f == -1 || player.target == player.system || player.system == homesystem || player.target == homesystem)
	{	// no target or bad target - don't launch
		interface_popup(font_6x8, SM_INV_X + 32*(SM_INV_X==0) - 64*(SM_INV_X>0), SM_INV_Y+40, 192, 80, STARMAP_INTERFACE_COLOR, 0, 
										title, textstring[STR_MIRROR_NOTARGET], textstring[STR_OK]);
		return;
	}

	if (sm_fleets[f].race == race_unknown)
	{	// space hulk
		c = 0;
		if (!(rand()%5))
			c = 1 + rand()%7;
		Play_Sound(WAV_MUS_COMBAT, 15, 1);
		interface_popup(font_6x8, SM_INV_X + 32*(SM_INV_X==0) - 64*(SM_INV_X>0), SM_INV_Y+40, 192, 80, STARMAP_INTERFACE_COLOR, 0, 
										title, textstring[STR_MIRROR_NOCANDO1+c], textstring[STR_OK]);
		Play_Sound(WAV_BRIDGE, 15, 1, 50);
		return;
	}

	c = sm_fleets[f].system;
	sm_fleets[f].system =	player.system;
	player.system = c;

	for (c = 0; c < STARMAP_MAX_FLEETS; c++)
	{
		if (f != c && sm_fleets[c].num_ships>0 && sm_fleets[c].race==race_klakar && sm_fleets[c].system == sm_fleets[f].system)
		{
			sm_fleets[c].system = player.system;
		}
	}

	player.x = sm_stars[player.system].x;
	player.y = sm_stars[player.system].y;

	player.explore = 1;
	starmap_advancedays(1);
	
	Play_SoundFX(WAV_MIRROR, 0);

	if (sm_stars[player.system].explored<2)
	{
		c = get_ik_timer(0); t = c;
		while (t - c < 75 && !must_quit)
		{
			ik_eventhandler();

			if (get_ik_timer(0)> t)
			{
				t = get_ik_timer(0);

				prep_screen();
				starmap_display(t);
				ik_blit();
			}
		}
	}
	must_quit = 0;

}

int32 eledras_bauble(char *title)
{
	int32 h;
	int32 end;
	int32 bx = 224, by = 112;
	int32 tof = 0;
	int32 it;
	int32 mc, c, x;
	int32 t;
	char itname[256];
	char texty[256];

	if (!interface_popup(font_6x8, SM_INV_X + 32*(SM_INV_X==0) - 64*(SM_INV_X>0), SM_INV_Y+40, 192, 80, STARMAP_INTERFACE_COLOR, 0, 
			title, textstring[STR_BAUBLE_CONFIRM], textstring[STR_YES], textstring[STR_NO]))
	{
		Stop_All_Sounds();

		Play_SoundFX(WAV_FOMAX_HI, 0);
		h = 120;

		prep_screen();
		halfbritescreen();
		interface_drawborder(screen,
												 bx, by, bx+192, by+h,
												 1, STARMAP_INTERFACE_COLOR, textstring[STR_BAUBLE_FOMAX]);

		ik_dsprite(screen, bx+16, by+24, spr_SMraces->spr[11], 0);
		ik_dsprite(screen, bx+16, by+24, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
		interface_textbox(screen, font_4x8,
											bx+84, by+24, 92, 64, 0,
											textstring[STR_BAUBLE_WISH]);

		ik_print(screen, font_6x8, bx+16, by+92, 3, textstring[STR_BAUBLE_PROMPT]);

		ik_blit();

		sprintf(itname, "");
		ik_text_input(bx+16, by+104, 26, font_6x8, "", itname, STARMAP_INTERFACE_COLOR*16+3, STARMAP_INTERFACE_COLOR);

		it = -1;
		for (c = 0; c < num_itemtypes; c++)
			if (!strcmp(itname, itemtypes[c].name))
				it = c;

		if (it==-1)
		{
			for (c = 0; c < num_itemtypes; c++)
			if (strlen(itname) <= strlen(itemtypes[c].name))
			{
				for (x = 0; x <= (int)(strlen(itemtypes[c].name)-strlen(itname)); x++)
				if (!_strnicmp(itname, itemtypes[c].name + x, strlen(itname)))
				{	
					it = c;		
					break;
				}

				if (it > -1)
					break;
			}

		}
/*
		rules of wishing:

	- no muktian ambassador or klakar beacon
	- no duplicates of artifacts (.clas = "artifact") -> no eledra's bauble (obviously)
*/

		if (it>-1)	
			if (itemtypes[it].flag & 2) // unsellable (klakar beacon, ambassador)
				it = -1;

		if (it>-1)
			if (!stricmp(itemtypes[it].clas, textstring[STR_INV_ARTIFACT]))	// check for duplicate artifacts
			{
				for (c = 0; c < kla_numitems; c++)
					if (kla_items[c] == it)
						it = -1;
				for (c = 0; c < player.num_items; c++)
					if (player.items[c] == it)
						it = -1;
				for (c = 0; c < num_stars; c++)
					if (it > -1 && c != homesystem && !strcmp(ecards[sm_stars[c].card].name, itemtypes[it].name))
						it = -1;
			}

		x = 0;
		while (it == -1) // didn't find an item, produce random lifeform
		{
			it = rand()%num_itemtypes;
			if (itemtypes[it].flag & 2)
				it = -1;
			else if (itemtypes[it].type != item_lifeform)
				it = -1;
			else
			{
				for (c = 0; c < kla_numitems; c++)
					if (kla_items[c] == it)
						it = -1;
				for (c = 0; c < player.num_items; c++)
					if (player.items[c] == it)
						it = -1;
				for (c = 0; c < num_stars; c++)
					if (it > -1 && c != homesystem && !strcmp(ecards[sm_stars[c].card].name, itemtypes[it].name))
						it = -1;
			}
			x = 1;
		}

		if (it>-1)
		{
			player.items[player.num_items++]=it;
			if (x)
			{
				sprintf(texty, textstring[STR_BAUBLE_FAIL], itemtypes[it].name);
				Play_SoundFX(WAV_FOMAX_BYE, 0);
			}
			else
			{
				x = itemtypes[it].name[0];
				if (x=='A' || x=='E' || x=='I' || x=='O' || x=='U')
					sprintf(itname, textstring[STR_BAUBLE_AN], itemtypes[it].name);
				else
					sprintf(itname, textstring[STR_BAUBLE_A], itemtypes[it].name);
				sprintf(texty, textstring[STR_BAUBLE_GIFT], itname);
				Play_SoundFX(WAV_FOMAX_WISH, 0);
			}
			
			prep_screen();
			interface_drawborder(screen,
													 bx, by, bx+192, by+h,
													 1, STARMAP_INTERFACE_COLOR, textstring[STR_BAUBLE_FOMAX]);

			ik_dsprite(screen, bx+16, by+24, spr_SMraces->spr[11], 0);
			ik_dsprite(screen, bx+16, by+24, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
			interface_textbox(screen, font_4x8,
												bx+84, by+24, 92, 64, 0,
												texty);

			interface_drawbutton(screen, bx+144, by+h-24, 32, STARMAP_INTERFACE_COLOR, textstring[STR_OK]);
			ik_blit();

			end = 0;
			while (!must_quit && !end)
			{
				ik_eventhandler();  // always call every frame
				mc = ik_mclick();	
				c = ik_inkey();

				if (mc == 1)
				{
					if (ik_mouse_y > by+h-24 && ik_mouse_y < by+h-8 && ik_mouse_x > bx+144 && ik_mouse_x < bx+176)
					{	end = 1; Play_SoundFX(WAV_DOT, get_ik_timer(0)); }
				}

				c = t; t = get_ik_timer(2);
				if (t != c)
				{ prep_screen(); ik_blit();	}
			}
		}

		must_quit = 0;
		reshalfbritescreen();

		return 1;
	}
	must_quit = 0;

	return 0;
}

void use_conograph(char *title)
{
	Play_Sound(WAV_CONOGRAPH2, 14, 1);
	Play_Sound(WAV_CONOGRAPH, 15, 1);
	interface_popup(font_6x8, SM_INV_X + 32*(SM_INV_X==0) - 64*(SM_INV_X>0), SM_INV_Y+40, 192, 80, STARMAP_INTERFACE_COLOR, 0, 
									title, textstring[STR_CONOGRAPH_PLAY], textstring[STR_OK]);

	starmap_advancedays(365);
	Stop_All_Sounds();
}
#endif

int32 item_colorcode(int32 it)
{
	switch (itemtypes[it].type)
	{
		case item_weapon:
		return 1;
		break;

		case item_system:
		switch (shipsystems[itemtypes[it].index].type)
		{
			case sys_thruster:
			return 2;

			case sys_engine:
			return 3;

			default:
			return 5;
		}
		break;

		case item_device:
		return 6;
		break;

		case item_lifeform:
		return 4;
		break;

		case item_treasure:
		return 7;
		break;

		default: 
		return 0;
	}

	return 0;
}