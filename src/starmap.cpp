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

t_player		player;
t_hud				hud;

int starmap_tutorialtype;

// ----------------
// LOCAL VARIABLES
// ----------------

int32 kawangi_score;
int32 kawangi_splode;
int32 kawangi_incoming;
int32 timer_warning;

// ----------------
// LOCAL PROTOTYPES
// ----------------

void help_screen();

void starmap_displayship(int32 t, int32 st);
int starmap_findstar(int32 mx, int32 my);


void killstar(int32 c);

void starmap_flee();
int32 simulate_move(int32 star);

// ----------------
// GLOBAL FUNCTIONS
// ----------------

void starmap()
{
	int32 t, t0;
	int32 c, mc;
	int32 mx, my;
	int32 mx2, my2;
	int32 d, s;
	int32 upd;
	int32 sp1, sp2;
	int32 end = 0;
	int32 bu[3], nbu;
	char texty[256];
	char topic[32];
	char hisher[8];

	kawangi_score = 0;
	kawangi_splode = 0;
	kawangi_incoming = 0;
	timer_warning = 0;

	ik_inkey();
	start_ik_timer(0, 1000/STARMAP_FRAMERATE); t0 = t = 0; upd=1;
	Play_Sound(WAV_BRIDGE, 15, 1, 50);

	starmap_tutorialtype = tut_starmap;

	while (!must_quit && !end)
	{
		t0 = t;
		ik_eventhandler();  // always call every frame
		t = get_ik_timer(0);
		c = ik_inkey();
		mc = ik_mclick();	
		mx = ik_mouse_x; my = ik_mouse_y;

		if (must_quit)
		{
			must_quit = 0;
			Play_SoundFX(WAV_DESELECT);
			if (!interface_popup(font_6x8, 240, 200, 160, 72, STARMAP_INTERFACE_COLOR, 0, 
					textstring[STR_QUIT_TITLE], textstring[STR_QUIT_CONFIRM], 
					textstring[STR_YES], textstring[STR_NO]))
			{	must_quit = 1; player.death = 666; }
		}

		if (!player.enroute && player.num_ships>0)
		{
			if ((c == 13 || c == 32) && player.target > -1 && player.target != player.system)
			{	player.engage = 1; player.fold = 0; }

			if (key_pressed(key_f[0]))
			{	
				help_screen();
				upd = 1;
			}

			if (mx > SM_MAP_X && mx < SM_MAP_X + 480 && my < SM_MAP_Y + 480)
			{
			if (my < SM_MAP_Y + 16 && (mc&1))
			{
				if (!settings.opt_timeremaining || player.stardate >= 365*10)
				{
					if (mx > SM_MAP_X+394 && mx < SM_MAP_X+406)
					{	
						Play_SoundFX(WAV_WAIT);
						starmap_advancedays(1);
					}
					else if (mx > SM_MAP_X+412 && mx < SM_MAP_X+430)
					{
						Play_SoundFX(WAV_WAIT);
						s = 0;
						for (d = 0; d < 12; d++)
							if (player.stardate%365 >= months[d].sd)
								s = d;
						starmap_advancedays(months[s].le);
					}
					else if (mx > SM_MAP_X+436 && mx < SM_MAP_X+460)
					{
						Play_SoundFX(WAV_WAIT);
						starmap_advancedays(365);
					}
				}
				else
				{
					if (mx > SM_MAP_X+436 && mx < SM_MAP_X+460)
					{
						s = (SM_MAP_X+459-mx)/6;
						d = 1;
						while (s--)
							d*=10;
						Play_SoundFX(WAV_WAIT);
						starmap_advancedays(d);
					}
				}
			}
			else
			{
				if (mc & 1)
				{
					// check for engage or fold space
					mx2 = 0;
					bu[0] = 0; nbu = 1;
					if (player.distance <= 2380)
					{
						for (d = 0; d < player.num_ships; d++)
							if (shiptypes[player.ships[d]].flag & 4)	// kuti
								bu[nbu++] = 1;
						if (player.target>-1 && sm_stars[player.target].explored==0)
							for (d = 0; d < shiptypes[0].num_systems; d++)
								if (shipsystems[shiptypes[0].system[d]].type == sys_misc && shipsystems[shiptypes[0].system[d]].par[0]==0)
									bu[nbu++] = 2;
					}

					if (player.target>-1 && player.target!=player.system)
					if (mx > sm_stars[player.target].ds_x-16 && mx < sm_stars[player.target].ds_x+16)
					{
						d = (sm_stars[player.target].y<sm_stars[player.system].y)-(sm_stars[player.target].y>=sm_stars[player.system].y);
						my2 = sm_stars[player.target].ds_y + (12+nbu*5)*d - nbu*5;
						if (my >= my2 && my < my2 + nbu*10)
						{
							d = (my - my2) / 10;
							if (bu[d] == 0 || bu[d] == 1)
							{
								player.engage = 1;
								if (bu[d] == 1)
								{	// fold space
									player.fold = 1; 

								}
								else
								{	
									player.fold = 0; 
								}
							}
							else if (bu[d] == 2)	// scan
							{
								Stop_Sound(15);
								probe_exploreplanet(1);
								Play_Sound(WAV_BRIDGE, 15, 1, 50);
							}
							mx2 = 1;
						}
					}
					if (player.target == player.system) 				// hire and hunt buttons
					if (mx > sm_stars[player.target].ds_x-16 && mx < sm_stars[player.target].ds_x+16)
					{
						d = c; 
						c = sm_stars[player.target].card;
						if (my > sm_stars[player.target].ds_y+11 && my < sm_stars[player.target].ds_y+23)
						{
							if (ecards[c].type == card_ally) 
							{
								mx2 = 1;
								Play_Sound(WAV_ALLY, 15, 1);
								sprintf(texty, textstring[STR_MERC_DEAL], hulls[shiptypes[ecards[c].parm].hull].name, shiptypes[ecards[c].parm].name);
								if (!interface_popup(font_6x8, 224, 192, 192, 96, STARMAP_INTERFACE_COLOR, 0, 
																		textstring[STR_MERC_TITLE], texty, textstring[STR_YES], textstring[STR_NO]))
								{
									Stop_Sound(15);
									player.ships[player.num_ships] = ecards[c].parm;
									//player.sel_ship = player.num_ships;
									player.num_ships++;
									starmap_tutorialtype = tut_ally;

									if (shiptypes[player.ships[player.num_ships-1]].flag & 64)
										sprintf(hisher, textstring[STR_MERC_HER]);
									else
										sprintf(hisher, textstring[STR_MERC_HIS]);

									sprintf(texty, textstring[STR_MERC_PAYMENT], 
													hulls[shiptypes[player.ships[player.num_ships-1]].hull].name,
													shiptypes[player.ships[player.num_ships-1]].name,
													hisher);
									if (pay_item(textstring[STR_MERC_BILLING], texty, shiptypes[ecards[c].parm].race) == -1)
									{
										starmap_removeship(player.num_ships-1);
										starmap_tutorialtype = tut_starmap;
									}
									else
										sm_stars[player.target].card = 0;
									player.sel_ship = 0;
								}
								Play_Sound(WAV_BRIDGE, 15, 1, 50);
							}
#ifndef DEMO_VERSION
							else if	(ecards[c].type == card_lifeform)
							{
								mx2 = 1;
								Play_SoundFX(WAV_LIFEFORM);
								s = ecards[c].parm;
								if (s > -1 && itemtypes[s].flag && lifeform_hard)
								{
									my2 = itemtypes[s].cost/10 + (player.target*7)%(itemtypes[s].cost/10);
									starmap_additem(s, 0);
									sprintf(texty, textstring[STR_LIFEFORM_HUNT], itemtypes[s].name, my2);
									if (!interface_popup(font_6x8, 224, 192, 192, 0, STARMAP_INTERFACE_COLOR, 0, textstring[STR_LIFEFORM_HUNTT], texty, textstring[STR_YES], textstring[STR_NO]))
									{
										starmap_advancedays(my2);
										sm_stars[player.system].card = 0;
									}
									else
										starmap_removeitem(player.num_items-1);
									Stop_All_Sounds();
								}
								Play_Sound(WAV_BRIDGE, 15, 1, 50);
							}
#endif
						}
						c = d;
					}

					// if no movement, check for new selection
					if (!mx2)
					{
						d = starmap_findstar(mx, my);
						player.target = d;
						if (player.target > -1)
						{
							player.distance = starmap_stardist(player.system, player.target);
							player.nebula = starmap_nebuladist(player.system, player.target);
							Play_SoundFX(WAV_SELECTSTAR);
						}
						else
							Play_SoundFX(WAV_DESELECT);
					}
				}
			}
			}
			else
			{
				if (mx > SM_SHIP_X && mx < SM_SHIP_X + 160 && my > SM_SHIP_Y + 24 && my < SM_SHIP_Y + 256)
				{
					mx2 = mx - SM_SHIP_X; my2 = my - SM_SHIP_Y;
					if (mc & 1)
					{
						// select ship
						if (mx > SM_SHIP_X + 16 && mx < SM_SHIP_X + 80 && my > SM_SHIP_Y + 24 && my < SM_SHIP_Y + 40)
						{
							d = (mx-16-SM_SHIP_X)/16;
							if (d < player.num_ships)
							{	player.sel_ship = d; player.sel_ship_time = t; Play_SoundFX(WAV_SELECTSHIP,t); }
						}
						// repair hull
						if (mx > SM_SHIP_X + 80 && mx < SM_SHIP_X + 144 && my > SM_SHIP_Y + 24 && my < SM_SHIP_Y + 40)
						{
							if (shiptypes[player.ships[player.sel_ship]].hits < hulls[shiptypes[player.ships[player.sel_ship]].hull].hits*256)
							{
								sp1 = (hulls[shiptypes[player.ships[player.sel_ship]].hull].hits*256 - shiptypes[player.ships[player.sel_ship]].hits) / 128;
								for (d = 0; d < shiptypes[player.ships[player.sel_ship]].num_systems; d++)
									if (shipsystems[shiptypes[player.ships[player.sel_ship]].system[d]].type == sys_damage)
										sp1 = 2;

								Play_SoundFX(WAV_INSTALL, t);
								sprintf(texty, textstring[STR_INV_REPAIR_HULL], sp1);
								sp2 = interface_popup(font_6x8, SM_SHIP_X + 32*(SM_SHIP_X==0) - 64*(SM_SHIP_X>0), SM_SHIP_Y+40, 192, 72, STARMAP_INTERFACE_COLOR, 0, 
																		textstring[STR_INV_REPAIR_TITLE], texty, textstring[STR_YES], textstring[STR_NO]);
								if (!sp2)
								{
									starmap_advancedays(sp1);
									upd=1;
								}
							}
						}
						// uninstall
						if (mx2 > 24 && mx2 < 144 && my2 > 168 && my2 < 168 + shiptypes[player.ships[player.sel_ship]].num_systems*8)
						{
							if (mx2 > 32 && mx2 < 144) // get info
							{
								upd = shipsystems[shiptypes[player.ships[player.sel_ship]].system[(my2-168)/8]].item;
								if (upd > -1)
								{
									Play_SoundFX(WAV_INFO);
									interface_popup(font_6x8, SM_SHIP_X + 32*(SM_SHIP_X==0) - 64*(SM_SHIP_X>0), SM_SHIP_Y+24, 192, 112, STARMAP_INTERFACE_COLOR, 0, 
																	itemtypes[upd].name, itemtypes[upd].text, textstring[STR_OK]);
									upd=1;
								}
								else
								{
									Play_SoundFX(WAV_DESELECT, t);
									upd = 1;
								}
							}
							if (mx2 > 24 && mx2 < 32)
							{
								if (!shiptypes[player.ships[player.sel_ship]].sysdmg[(my2-168)>>3])
								{
									if (player.sel_ship == 0)
									{
										Play_SoundFX(WAV_INSTALL, t);					
										starmap_uninstallsystem( (my2-168)>>3, 0);
									}
								}
								else	// repair damages
								{
									sp1 = (int32)(sqrt(itemtypes[shipsystems[shiptypes[player.ships[player.sel_ship]].system[(my-168)>>3]].item].cost)*.75);
									sprintf(texty, textstring[STR_INV_REPAIR_SYS], itemtypes[shipsystems[shiptypes[player.ships[player.sel_ship]].system[(my-168)>>3]].item].name, sp1);
									Play_SoundFX(WAV_INSTALL, t);
									sp2 = interface_popup(font_6x8, SM_SHIP_X + 32*(SM_SHIP_X==0) - 64*(SM_SHIP_X>0), SM_SHIP_Y+160, 192, 72, STARMAP_INTERFACE_COLOR, 0, 
																	textstring[STR_INV_REPAIR_TITLE], texty, textstring[STR_YES], textstring[STR_NO]);
									if (!sp2)
									{
										shiptypes[player.ships[player.sel_ship]].sysdmg[(my-168)>>3] = 0;
										starmap_advancedays(sp1);
										upd=1;
									}
								}
							}
						}
					}
				}
				if (mx > SM_INV_X && mx < SM_INV_X + 160 && my > SM_INV_Y + 24 && my < SM_INV_Y + 136)
				{
					mx2 = mx - SM_INV_X; my2 = my - SM_INV_Y;
					if (mc & 1)
					{
						/*if (player.sel_ship != 0)
						{
							
							player.sel_ship = 0;
							prep_screen();
							starmap_display(t);
							ik_blit();
						} */
						if (mx2 > 16 && mx2 < 136 && my2 < 120)
						{
							d = (my2 - 24)/8 + hud.invslider;
							if (d < player.num_items)
							{
								hud.invselect = d;
								if ( mx2 < 24 )
								{
									if (itemtypes[player.items[d]].type == item_system || itemtypes[player.items[d]].type == item_weapon)
									{
										if (player.sel_ship == 0)	// your ship
										{
											Play_SoundFX(WAV_INSTALL, t);
											starmap_installitem(hud.invselect);
											upd=1;
										}
										else
										{
											Play_SoundFX(WAV_DESELECT);
											if (ally_install(player.sel_ship, player.items[d], 0) > -1)
												starmap_removeitem(d);
										}
									}
									else if (itemtypes[player.items[d]].type == item_device)
									{
										Play_SoundFX(WAV_INFO);
										d = itemtypes[player.items[hud.invselect]].flag;
										if (d & device_beacon)
										{	// beacon
											Stop_Sound(15);
											if (!sm_stars[player.system].novadate || sm_stars[player.system].novatype==2)
												klakar_trade();
											else
											{
												Play_SoundFX(WAV_INFO);
												Play_Sound(WAV_MESSAGE, 15, 1);
												sprintf(texty, textstring[STR_KLAK_UNSAFE], sm_stars[player.system].starname);
												interface_popup(font_6x8, SM_INV_X + 32*(SM_INV_X==0) - 64*(SM_INV_X>0), SM_INV_Y+24, 192, 112, STARMAP_INTERFACE_COLOR, 0, 
																		textstring[STR_KLAK_UNAVAIL], texty, textstring[STR_OK]);
												upd=1;
											}
											Play_Sound(WAV_BRIDGE, 15, 1, 50);
										}
										else if (d & device_probe)
										{
											Stop_Sound(15);
											if (stellar_probe(itemtypes[player.items[hud.invselect]].name))
												starmap_removeitem(hud.invselect);
											Play_Sound(WAV_BRIDGE, 15, 1, 50);
										}
#ifndef DEMO_VERSION
										else if (d & device_collapser)
										{
											if (use_vacuum_collapser(itemtypes[player.items[hud.invselect]].name))
											{
												starmap_removeitem(hud.invselect);
											}
//											end = 1; player.death = 3;
										}
										else if (d & device_mirror)
										{
											Stop_Sound(15);
											eledras_mirror(itemtypes[player.items[hud.invselect]].name);
											Play_Sound(WAV_BRIDGE, 15, 1, 50);
										}
										else if (d & device_bauble)
										{
											Stop_Sound(15);
											if (eledras_bauble(itemtypes[player.items[hud.invselect]].name))
												starmap_removeitem(hud.invselect);
											Play_Sound(WAV_BRIDGE, 15, 1, 50);
										}
										else if (d & device_conograph)
										{
											Stop_Sound(15);
											use_conograph(itemtypes[player.items[hud.invselect]].name);
											Play_Sound(WAV_BRIDGE, 15, 1, 50);
										}
#endif
									}
								}
								else 
								{
									Play_SoundFX(WAV_INFO);
									interface_popup(font_6x8, SM_INV_X + 32*(SM_INV_X==0) - 64*(SM_INV_X>0), SM_INV_Y+24, 192, 112, STARMAP_INTERFACE_COLOR, 0, 
																	itemtypes[player.items[hud.invselect]].name, itemtypes[player.items[hud.invselect]].text, textstring[STR_OK]);
									upd=1;
								}
							}
							else
							{
								hud.invselect = -1;
								Play_SoundFX(WAV_DESELECT);
							}
						}
						else if (mx2 > 136 && mx2 < 144 && my2 < 120)
						{
							if (player.num_items > 12)
							{
								Play_SoundFX(WAV_SLIDER);
								if (my2 > 32 && my2 < 112)
									hud.invslider = MIN(((my2 - 32)*(player.num_items-12)+40) / 80, player.num_items-12);
								else if (my2 < 32)
									hud.invslider = MAX(0, hud.invslider-1);
								else if (my2 > 112)
									hud.invslider = MIN(player.num_items-12, hud.invslider+1);
							}
						}
					}
				}
				
			}
		
			if (player.engage)
			{
				if (player.target == homesystem) // going home
				{
					Play_SoundFX(WAV_DESELECT);
					if (!interface_popup(font_4x8, 240, 200, 160, 72, STARMAP_INTERFACE_COLOR, 0, 
							textstring[STR_ENDGAME_CONFIRM1], textstring[STR_ENDGAME_CONFIRM2], textstring[STR_YES], textstring[STR_NO]))
					{
						t0 = t;
						t = get_ik_timer(0);
						Stop_All_Sounds();
						player.enroute = 1;
					}
				}
				else
				{
					player.enroute = 1;
				}
				if (player.engage != 2)
				if ((shiptypes[0].engine==-1 || shiptypes[0].sysdmg[shiptypes[0].sys_eng]) && !player.fold)
				{
					if (shiptypes[0].engine==-1)
					{
						sprintf(topic, textstring[STR_DRIVE_MISSING]);
						sprintf(texty, textstring[STR_DRIVE_MISSING2]);
					}
					else
					{
						sprintf(topic, textstring[STR_DRIVE_BROKEN]);
						sprintf(texty, textstring[STR_DRIVE_BROKEN2]);
					}

					Play_SoundFX(WAV_DESELECT);
					if (!interface_popup(font_4x8, 240, 200, 160, 72, STARMAP_INTERFACE_COLOR, 0, 
							topic, texty, textstring[STR_YES], textstring[STR_NO]))
					{
						t0 = t;
						t = get_ik_timer(0);
						Stop_All_Sounds();
						player.enroute = 1;
					}
					else
						player.enroute = 0;
				}
				if (player.enroute)
				{
#ifndef DEMO_VERSION
					if (player.fold || (shiptypes[0].engine>-1 && shipsystems[shiptypes[0].engine].par[0]==666))
					{	
						if (player.fold)
						{
							if (player.stardate - player.foldate < 7)
							{
								if (sm_stars[player.system].novadate>0 && player.stardate - player.hypdate < 60 )
								{	// attempting to hyper out of nova
									Play_SoundFX(WAV_DESELECT);
									if (!interface_popup(font_4x8, 240, 200, 160, 72, STARMAP_INTERFACE_COLOR, 0, 
											textstring[STR_DRIVE_NOVA1], textstring[STR_DRIVE_NOVA3], textstring[STR_YES], textstring[STR_NO]))
									{
										t0 = t;
										t = get_ik_timer(0);
										Stop_All_Sounds();
									}
									else
										player.enroute = 0;
								}
								if (player.enroute)
								{
									starmap_advancedays(7 - (player.stardate - player.foldate));
									prep_screen();
									starmap_display(t);
									ik_blit();
								}
							}
							if (player.enroute)
							{
								player.foldate = player.stardate;
								Play_SoundFX(WAV_FOLDSPACE, t);
							}
						}
						else
						{
							if (player.stardate - player.hypdate < 60)
							{	
								if (sm_stars[player.system].novadate>player.stardate && player.stardate - player.hypdate < 60 )
								{	// attempting to hyper out of nova
									Play_SoundFX(WAV_DESELECT);
									if (!interface_popup(font_4x8, 240, 200, 160, 72, STARMAP_INTERFACE_COLOR, 0, 
											textstring[STR_DRIVE_NOVA1], textstring[STR_DRIVE_NOVA2], textstring[STR_YES], textstring[STR_NO]))
									{
										t0 = t;
										t = get_ik_timer(0);
										Stop_All_Sounds();
									}
									else
										player.enroute = 0;
								}
								if (player.enroute)
								{
									starmap_advancedays(60 - (player.stardate - player.hypdate));
									prep_screen();
									starmap_display(t);
									ik_blit();
								}
							}
							if (player.enroute)
							{
								player.hypdate = player.stardate;
								Play_SoundFX(WAV_HYPERDRIVE, t);
							}
						}

						if (player.enroute)
							player.hyptime = t;
					}
					else
#endif
					{
						Play_SoundFX(WAV_DEPART, t);
					}
					ik_print_log("Set course for %s system.\n", sm_stars[player.target].starname);
				}

				player.engage = 0;
			}
		}

		if (t>t0 || upd)
		{
			if (upd)
				upd = 0;

			if (player.enroute)
			{
				s = player.enroute;
				if (shiptypes[0].engine > -1 && shiptypes[0].sysdmg[shiptypes[0].sys_eng]==0)
				{
					sp1 = shipsystems[shiptypes[0].engine].par[0];
					sp2 = shipsystems[shiptypes[0].engine].par[1];
				}
				else if (shiptypes[0].thrust > -1 && shiptypes[0].sysdmg[shiptypes[0].sys_thru]==0)
				{
					sp1 = 1; sp2 = 1;
				}
				else
				{
					sp1 = 0; sp2 = 0;
				}

				if (player.fold)	// fold
				{
					if (t > player.hyptime + 125)
					{
						player.enroute = player.distance;
					}
				}
				else if (sp1 == 666)		// hyperdrive
				{
					if (t > player.hyptime + 125)
					{
						player.enroute = player.distance;
					}
				}
				else	// normal drives
				{
					if (sp1 > 0 && sp2 > 0)
					{
						if (sm_nebulamap[((240-player.y)<<9)+(240+player.x)])
							player.enroute += sp2*2;
						else
							player.enroute += sp1*2;
						starmap_advancedays(2);
					}
					else
					{
						player.enroute += 1;
						starmap_advancedays(4);
					}
					if (player.enroute >= player.distance-640 && s < player.distance-640)
						Play_SoundFX(WAV_ARRIVE, t);

					// find black holes?
					for (s = 0; s < num_holes; s++)
					if (sm_holes[s].size>0)
					{
						if (!sm_holes[s].explored)
						{
							d = get_distance(sm_holes[s].x - player.x, sm_holes[s].y - player.y);
							if (d < 32)
							{
								Stop_Sound(15);
								d = starmap_explorehole(s, t);
								Play_Sound(WAV_BRIDGE, 15, 1, 50);
								// returns 0 if [go back], 1 if [continue]
								if (!d) // turn around
								{
									player.enroute = player.distance - player.enroute;
									d = player.system;
									player.system = player.target; 
									player.target = d;
								}
							}
		//						sm_holes[s].explored = 1;
						}
						else
						{
							d = get_distance(sm_holes[s].x - player.x, sm_holes[s].y - player.y);
							if (sp1>0)
							{
								if (d < 96/(MAX(sp1,6)) )
								{
			//						player.num_ships = 0;
									player.death = 2;
									player.hole = s;
									end = 1;
								}
							}
							else if (d < 96/6)
							{
		//						player.num_ships = 0;
								player.death = 2;
								player.hole = s;
								end = 1;
							}
						}
					}
				}

				// arrive at destination?
				if (player.enroute >= player.distance)
				{
					//d = ((player.distance*(256-player.nebula)*365)/sp1 + (player.distance*player.nebula*365)/sp2)>>16;
					//player.stardate += d;

					player.system = player.target;
					player.x = sm_stars[player.system].x;
					player.y = sm_stars[player.system].y;
					player.enroute = 0;
					player.distance = 0;
					player.explore = 1;

					ik_print_log("Arrived at %s system.\n", sm_stars[player.system].starname);
/*
					if (!sm_stars[player.target].explored)
						player.card = rand()%num_ecards;
					sm_stars[player.target].explored = 1;
*/
				}
			}

			resallhalfbritescreens();	// cleanup

			prep_screen();
//		interface_popup(font_6x8, 0,0,128,64,0,0,"hump", "blarg", "ok");
			starmap_display(t);

			ik_blit();

			if (settings.random_names & 4)
				interface_tutorial(starmap_tutorialtype);
		}

		if (player.explore==1)
		{
			Stop_Sound(15);
			player.explore = 0;
			s = starmap_entersystem();
			if ((player.num_ships == 0 || player.ships[0] != 0) && player.death == 0)
			{	end = 1; player.death = 1; }
			else if (player.death)
			{	end = 1; }
			else if (s)
			{
				prep_screen();
				starmap_display(t);
				ik_blit();
				starmap_exploreplanet();
				if (player.system == homesystem)
				{	end = 1; }
			}
			else	// flee
			{
				starmap_flee();
			}
			upd=1;
			Play_Sound(WAV_BRIDGE, 15, 1, 50);
		}

#ifndef DEMO_VERSION
		for (s = 0; s < num_stars; s++)
			if (sm_stars[s].color > -2 && sm_stars[s].novatype == 2 &&
					player.stardate >= sm_stars[s].novadate)
			{
				vacuum_collapse(s);
			}

		for (s = 0; s < STARMAP_MAX_FLEETS; s++)
		if (sm_fleets[s].race == race_kawangi) 
		{	
			if (sm_fleets[s].num_ships > 0 && sm_fleets[s].system == homesystem && sm_fleets[s].distance == 0)
			{
				kawangi_warning();
				sm_fleets[s].explored = 2;
				sm_fleets[s].distance = player.stardate + 365;
			}
			else if (sm_fleets[s].num_ships == 0)	// destroyed the kawangi
			{
				if (player.num_ships > 0)
					kawangi_message(s, 0);
				sm_fleets[s].race = race_unknown;
				player.bonusdata += 2000;
			}

			if (kawangi_splode)	// && sm_stars[sm_fleets[s].system].novatime>0 && get_ik_timer(2)>sm_stars[sm_fleets[s].system].novatime+100)
			{
				sm_stars[sm_fleets[s].system].novadate = player.stardate;
				sm_stars[sm_fleets[s].system].novatype = 1;
				kawangi_splode = 0;
				if (kawangi_score < 2)
				{
					kawangi_message(s, 1);
					Play_Sound(WAV_BRIDGE, 15, 1, 50);
				}
			}

			if (kawangi_incoming)
			{
				kawangi_incoming = 0;
				if (fleet_encounter(s, 1))
				{
					starmap_meetrace(sm_fleets[s].race);
					enemy_encounter(sm_fleets[s].race);
					must_quit = 0;
					combat(s, 0);
					player.sel_ship = 0;
					if ((player.num_ships == 0 || player.ships[0] != 0) && player.death == 0)
					{	player.death = 1; }
					if (sm_fleets[s].num_ships>0 && player.num_ships>0 && player.ships[0]==0)
					{	// flee
						starmap_flee();
					}
				}
				else
					starmap_flee();
			}

		}
#endif

		if (timer_warning)
		{
			if (timer_warning == 1)
			{
				Play_SoundFX(WAV_TIMER);
				interface_popup(font_6x8, 224, 200, 192, 80, STARMAP_INTERFACE_COLOR, 0, 
												textstring[STR_TIMER_TITLE], textstring[STR_TIMER_WARN1], textstring[STR_OK]);
			}
			else if (timer_warning == 2)
			{
				Play_SoundFX(WAV_TIMER);
				interface_popup(font_6x8, 224, 200, 192, 80, STARMAP_INTERFACE_COLOR, 0, 
												textstring[STR_TIMER_TITLE], textstring[STR_TIMER_WARN2], textstring[STR_OK]);
			}
			timer_warning = 0;
			Play_Sound(WAV_BRIDGE, 15, 1, 50);
		}

		if (player.num_ships == 0 || sm_stars[homesystem].planet == 10 || sm_stars[homesystem].color == -2)
			end=1;
		if (player.death == 666)
			must_quit=1;
	}

	// if quit, quit
	if (player.death == 666)
	{
		Stop_Sound(15);
		return;
	}

	// display effects of collapsers, black holes etc here
	if (end)
	{
		sp1 = player.stardate;
		s = t0 = t = get_ik_timer(0);
		player.deatht = get_ik_timer(2);
		end = 0;
		if (player.death == 2)
		{
			Play_SoundFX(WAV_BLACKHOLEDEATH, get_ik_timer(0));
			player.num_ships = 0;
		}
		else if (player.death == 1)
		{
			Play_SoundFX(WAV_EXPLO2, t);
			player.num_ships = 0;
		}
		else if (player.num_ships == 0 && player.death < 6)
		{
			Play_SoundFX(WAV_EXPLO2, t);
		}

		if (player.death == 7 || player.death == 3)
			end = 1;

		while (!end && !must_quit)
		{
			ik_eventhandler();
			t0 = t;
			t = get_ik_timer(0);

			ik_mclick();
			ik_inkey();

			if (t>t0)
			{
				if (player.death == 4)
					starmap_advancedays(2);
				prep_screen();
				starmap_display(t);
				ik_blit();
			}
			if (t-s > 200)
				end = 1;
			else if (t-s > 50 && player.system == homesystem && !player.enroute)
				end = 1;
		}

		player.stardate = sp1;
		Stop_Sound(15);
		game_over();
	}
}

// ----------------
// LOCAL FUNCTIONS
// ----------------

void starmap_displayship(int32 t, int32 st)
{
	int c, a;
	int cx, cy;
	int x, y;
	int u;
	int l;
	int s;
	int z;
	t_hull *hull;

	if (player.num_ships < 1)
		st = 0;

	u = 3*(st==0);	// only highlight uninstallers if player ship

	hull = &hulls[shiptypes[player.ships[player.sel_ship]].hull];
	interface_drawborder(screen,
											 SM_SHIP_X, SM_SHIP_Y, SM_SHIP_X + 160, SM_SHIP_Y + 256,
											 1, STARMAP_INTERFACE_COLOR, shiptypes[player.ships[player.sel_ship]].name);
	ik_dsprite(screen, SM_SHIP_X + 16, SM_SHIP_Y + 40, hull->silu, 2+(STARMAP_INTERFACE_COLOR<<8));

	// ship systems
	l = 160;
	cx = SM_SHIP_X; cy = SM_SHIP_Y;

//	ik_print(screen, font_4x8, SM_SHIP_X, SM_SHIP_Y, 3, "%d",shiptypes[player.ships[player.sel_ship]].num_systems);
	for (z = 0; z < shiptypes[player.ships[player.sel_ship]].num_systems; z++)
	{
		s = shiptypes[player.ships[player.sel_ship]].system[z];
		y = cy + (l+=8);
		if (!shiptypes[player.ships[player.sel_ship]].sysdmg[z])
			ik_dsprite(screen, cx + 24, y, spr_IFarrows->spr[12], 2+(u<<8));
		else
		{	
			ik_dsprite(screen, cx + 24, y, spr_IFarrows->spr[15], 2+((3-2*((t&31)> 24)) <<8));
		}
		switch(shipsystems[s].type)
		{
			case sys_weapon:				// weapons
				ik_print(screen, font_4x8, cx + 32, y, 1*(shipsystems[s].item>-1 && shiptypes[player.ships[player.sel_ship]].sysdmg[z]==0),   "%s", shipsystems[s].name);
				ik_drawline(screen, cx + 22, y + 3, cy + 20 - z*4, y + 3, 27, 0, 255);
				ik_drawline(screen, cx + 20 - z*4, y + 3, cy + 20 - z*4, cy + 46 - z*4, 27, 0, 255);
				ik_drawline(screen, cx + 20 - z*4, cy + 46 - z*4, cx + 16+hull->hardpts[z].x*2, cy + 46 - z*4, 27, 0, 255);
				ik_drawline(screen, cx + 16+hull->hardpts[z].x*2, cy + 46 - z*4, cx + 16+hull->hardpts[z].x*2, cy + 40 + hull->hardpts[z].y*2, 27, 0, 255);
			break;
			case sys_thruster:	// thrusters			
				ik_print(screen, font_4x8, cx + 32, y, 2*(shiptypes[player.ships[player.sel_ship]].sysdmg[z]==0),   "%s", shipsystems[s].name);
				ik_drawline(screen, cx + 34 + strlen(shipsystems[s].name)*4, y + 3, cx + 140, y + 3, 43, 0, 255);
				ik_drawline(screen, cx + 140, y + 3, cx + 140, cy + 166, 43, 0, 255);
				x = 64;
				for (c = 0; c < hulls[shiptypes[player.ships[player.sel_ship]].hull].numh; c++)
					if (hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].type==hdpThruster)
						x = MIN(x, hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].x);
				ik_drawline(screen, cx + 16+x*2, cy + 166, cx + 140, cy + 166, 43, 0, 255);
				for (c = 0; c < hulls[shiptypes[player.ships[player.sel_ship]].hull].numh; c++)
					if (hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].type==hdpThruster)
						ik_drawline(screen, cx + 16+hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].x*2, cy + 166, cx + 16 + hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].x*2, cy + 40 + hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].y*2, 43, 0, 255);
			break;
			case sys_engine:	// engine
				a = 64; x = 64;
				for (c = 0; c < hulls[shiptypes[player.ships[player.sel_ship]].hull].numh; c++)
					if (hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].type==hdpEngine)
					{	a = MIN(a, hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].y);
						x = MIN(x, hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].x);	}
				a -= 8;
				ik_print(screen, font_4x8, cx + 32, y, 3*(shiptypes[player.ships[player.sel_ship]].sysdmg[z]==0), "%s", shipsystems[s].name);
				ik_drawline(screen, cx + 34 + strlen(shipsystems[s].name)*4, y + 3, cx + 144, y + 3, 59, 0, 255);
				ik_drawline(screen, cx + 144, y + 3, cx + 144, cy+40+a*2, 59, 255);
				ik_drawline(screen, cx + 144, cy + 40 + a*2, cx + 16 + x*2, cy+40+a*2, 59, 255);
				for (c = 0; c < hulls[shiptypes[player.ships[player.sel_ship]].hull].numh; c++)
					if (hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].type==hdpEngine)
						ik_drawline(screen, cx + 16+hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].x*2, cy + 40 + a*2, cx + 16 + hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].x*2, cy + 40 + hulls[shiptypes[player.ships[player.sel_ship]].hull].hardpts[c].y*2, 59, 0, 255);
			break;
			
			default:	// misc systems
				ik_print(screen, font_4x8, cx + 32, y, 5*(shiptypes[player.ships[player.sel_ship]].sysdmg[z]==0), "%s", shipsystems[s].name);
		}
	}

	if (t > player.sel_ship_time && t < player.sel_ship_time + 32)
	{
		l = (32 - t + player.sel_ship_time)>>1;
		if (l>15) l=15;
		ik_drsprite(screen, SM_SHIP_X + 80, SM_SHIP_Y + 104, 
								0, 128, 
								hull->sprite, 5+(l<<8));
	}
	for (c = 0; c < hull->numh; c++)
	{
		l = 0;
		if (hull->hardpts[c].type == hdpWeapon && shipsystems[shiptypes[player.ships[player.sel_ship]].system[c]].item > -1)
			l=1;
		if (hull->hardpts[c].type == hdpEngine && shiptypes[player.ships[player.sel_ship]].engine>-1)
			l=3;
		if (hull->hardpts[c].type == hdpThruster && shiptypes[player.ships[player.sel_ship]].thrust>-1)
			l=2;
		ik_dsprite(screen, SM_SHIP_X + 8+hull->hardpts[c].x*2, SM_SHIP_Y + 32 + hull->hardpts[c].y*2, 
								spr_IFsystem->spr[hull->hardpts[c].type * 4 + 1], 2+(l<<8));
	}

	// draw all player ships (small)
	if (player.sel_ship > player.num_ships-1)
	{	player.sel_ship = 0; } //player.sel_ship_time = t; }
	for (c = 0; c < player.num_ships; c++)
	{
		l = 0;
		if (shiptypes[player.ships[c]].hits < hulls[shiptypes[player.ships[c]].hull].hits*256)
			l = 1;
		else
		for (s = 0; s < shiptypes[player.ships[c]].num_systems; s++)
		{
			if (shiptypes[player.ships[c]].sysdmg[s])
				l = 1;
		}

		if (l==0 || (t&31)>8)
			ik_drsprite(screen, SM_SHIP_X + 24 + c * 16, SM_SHIP_Y + 32, 0, 16, hulls[shiptypes[player.ships[c]].hull].sprite, 0);
		else
		{
			l = 26;
			ik_drsprite(screen, SM_SHIP_X + 24 + c * 16, SM_SHIP_Y + 32, 0, 16, hulls[shiptypes[player.ships[c]].hull].sprite, 1+(l<<8));
		}
	}
	l = (t&31)>23;
	ik_drsprite(screen, SM_SHIP_X + 24 + player.sel_ship * 16, SM_SHIP_Y + 32, 0, 16+l*2, spr_IFtarget->spr[8], 5+((8+l*7)<<8));

	// draw repair button
	c = player.sel_ship;
	if (shiptypes[player.ships[c]].hits < hulls[shiptypes[player.ships[c]].hull].hits*256)
		l = 1+(STARMAP_INTERFACE_COLOR-1)*((t&31)>8);
	else
		l = 0;
	//l = STARMAP_INTERFACE_COLOR*(shiptypes[player.ships[c]].hits < hulls[shiptypes[player.ships[c]].hull].hits*256);
	interface_drawbutton(screen, SM_SHIP_X+80, SM_SHIP_Y+24, 64, l, " ");
	ik_dsprite(screen, SM_SHIP_X+88, SM_SHIP_Y+24, spr_IFbutton->spr[16], 2+(l<<8));
	ik_dsprite(screen, SM_SHIP_X+120, SM_SHIP_Y+28, spr_IFbutton->spr[15], 2+(l<<8));
	if (shiptypes[player.ships[c]].hits/256 < hulls[shiptypes[player.ships[c]].hull].hits)
		ik_drawbox(screen, SM_SHIP_X+89+(MAX(shiptypes[player.ships[c]].hits,0)/256*22)/hulls[shiptypes[player.ships[c]].hull].hits, SM_SHIP_Y+28, 
							 SM_SHIP_X+110, SM_SHIP_Y+35,0);
	/*
	ik_drawmeter(screen, SM_SHIP_X+88, SM_SHIP_Y+29, SM_SHIP_X+112, SM_SHIP_Y+34, 1, 
			((shiptypes[player.ships[c]].hits/256) * 100) / hulls[shiptypes[player.ships[c]].hull].hits, 
			l, 28);
	*/


		//15

}

void starmap_display(int32 t)
{
	int c, a;
	int cx, cy;
	int x, y;
	int l, d;
	int sp1, sp2;
	char top[128];
	char lne[128];
	char cal[128];
	int bu[3], nbu;
	int nl;
	int ti2 = get_ik_timer(2);
	t_ik_sprite *fs1, *fs2;
	t_ik_sprite *ssp;
//	t_hull *hull;

	ssp = hulls[shiptypes[player.ships[0]].hull].sprite;
	
	// clear screen
	ik_drawbox(screen, 0, 0, 640, 480, 0);

	// draw starmap
	cy = SM_MAP_Y + 244;
	cx = SM_MAP_X + 240;
	
	ik_setclip(cx-232,cy-232,cx+232,cy+232);

	ik_copybox(sm_nebulagfx, screen, 8, 8, 472, 472, cx-232, cy-232);

	for (c = 0; c < num_holes; c++)
#ifndef STARMAP_DEBUGINFO
	if (sm_holes[c].explored && sm_holes[c].size>0)
	{
		ik_drsprite(screen, cx + sm_holes[c].x, cy - sm_holes[c].y,
								1023-((t*2)&1023), 32, spr_SMstars->spr[8], 4);
#else
	if (sm_holes[c].size>0)
	{
		ik_drsprite(screen, cx + sm_holes[c].x, cy - sm_holes[c].y,
								1023-((t*2)&1023), 32, spr_SMstars->spr[8], 5+((7+8*(sm_holes[c].explored>0))<<8) );
#endif
		if (player.explore != c+1)
		{
			if ( int32(cx+sm_holes[c].x + 12 + strlen(sm_holes[c].name)*4) < int32(cx + 232))
				ik_print(screen, font_4x8, cx + sm_holes[c].x + 12, cy - sm_holes[c].y - 3, 0, sm_holes[c].name);
			else
				ik_print(screen, font_4x8, cx + sm_holes[c].x - 12 - strlen(sm_holes[c].name)*4, cy - sm_holes[c].y - 3, 0, sm_holes[c].name);
		}
	}


	for (c = 0; c < num_stars; c++)
	if (sm_stars[c].color > -2)
	{
//	ik_dsprite(screen, 400 + (sm_stars[c].x>>8) - 16, 244 - (sm_stars[c].y>>8) - 16,
//					 spr_SMstars->spr[sm_stars[c].color], 0);
		sm_stars[c].ds_x = cx + sm_stars[c].x;
		sm_stars[c].ds_y = cy - sm_stars[c].y;

		if (sm_stars[c].color > -1)
		{
			ik_drsprite(screen, sm_stars[c].ds_x, sm_stars[c].ds_y,
									0, 32, spr_SMstars->spr[sm_stars[c].color], 4);
			ik_drsprite(screen, sm_stars[c].ds_x, sm_stars[c].ds_y,
									0, 32, spr_SMstars->spr[sm_stars[c].color], 2);
		}
		if ( int32(sm_stars[c].ds_x + 12 + strlen(sm_stars[c].starname)*4) < int32(cx + 232))
			ik_print(screen, font_4x8, sm_stars[c].ds_x + 12, sm_stars[c].ds_y - 3, 0, sm_stars[c].starname);
		else
			ik_print(screen, font_4x8, sm_stars[c].ds_x - 12 - strlen(sm_stars[c].starname)*4, sm_stars[c].ds_y - 3, 0, sm_stars[c].starname);
#ifdef STARMAP_DEBUGINFO
		if (sm_stars[c].card>-1 && c != homesystem)
			ik_print(screen, font_4x8, sm_stars[c].ds_x - 32, sm_stars[c].ds_y + 12, 0, ecards[sm_stars[c].card].name);
#endif

		if (sm_stars[c].novadate > 0)
		{
			nl = (5-4*sm_stars[c].novatype)*365;
		}

		if (sm_stars[c].novadate==0 || sm_stars[c].novadate>=player.stardate || sm_stars[c].novatype == 2)
		{
			if (sm_stars[c].explored && sm_stars[c].planet < 10)
			{
				a = ((t * (5+c/2)) & 1023);
				if (c&1)
					a = 1023 - a;
				ik_dsprite(screen, 
									 sm_stars[c].ds_x + (sin1k[a]>>12) - 8, 
									 sm_stars[c].ds_y - (cos1k[a]>>12) - 8, 
									 spr_SMplanet->spr[sm_stars[c].planet], 0);
				if (sm_stars[c].explored==1)
					ik_drsprite(screen, 
										 sm_stars[c].ds_x + (sin1k[a]>>12) , 
										 sm_stars[c].ds_y - (cos1k[a]>>12) , 
										 0, 16, spr_SMplanet->spr[sm_stars[c].planet], 
										 5+((15*((t&16)>0))<<8 ) );
			}

			if (ecards[sm_stars[c].card].type == card_ally && sm_stars[c].explored==2)
			{
				ik_drsprite(screen, 
										sm_stars[c].ds_x + (sin1k[(t*8+512) & 1023]>>12),
										sm_stars[c].ds_y - (cos1k[(t*8+512) & 1023]>>12),
										(t*8 + 768)&1023,
										24,
										hulls[shiptypes[ecards[sm_stars[c].card].parm].hull].sprite,
										0);
			}

			if (sm_stars[c].novadate > 0)
			{
				l = 8 + (sin1k[(t*12+4)&1023]>>13);
				l = MAX(MIN(l,15), 0);
				ik_drsprite(screen, sm_stars[c].ds_x, sm_stars[c].ds_y, (c*64+t*4)&1023, l+20, spr_shockwave->spr[4], 5+(l<<8));
			}

		//	ik_print(screen, font_4x8, sm_stars[c].ds_x - 4, sm_stars[c].ds_y + 8, 0, "%d", sm_stars[c].card);
		}
#ifndef DEMO_VERSION
		else if (sm_stars[c].novadate>0 && player.stardate < sm_stars[c].novadate+nl)
		{
			if (sm_stars[c].novatime == 0)
			{
				Play_SoundFX(WAV_EXPLO2);
				sm_stars[c].novatime = ti2;
			}
			a = ti2 - sm_stars[c].novatime;
			if (a < 50)
			{
				l = 15; 
				if (a > 35) l-=a-35;
				if (l < 0) l=0;
				ik_drsprite(screen, sm_stars[c].ds_x, sm_stars[c].ds_y, (c*64)&1023, a*2, spr_shockwave->spr[3], 5+(l<<8));
				l = 15;
				if (a > 15) l-=a-15;
				if (l>0)
					ik_drsprite(screen, sm_stars[c].ds_x, sm_stars[c].ds_y, (c*64)&1023, a*2+30, spr_shockwave->spr[4], 5+(l<<8));
			}

			a = player.stardate-sm_stars[c].novadate;
			if (sm_stars[c].planet != 10)  
			{	// destroy planet and everything
				sm_stars[c].color=7;
				sm_stars[c].planet = 10;
				sm_stars[c].card = 0;
				sm_stars[c].explored = 0;
				sprintf(sm_stars[c].planetname, "No Planets");
				while (plgfx_type[sm_stars[c].planetgfx] != sm_stars[c].planet && !must_quit)
				{
					ik_eventhandler();
					sm_stars[c].planetgfx = rand()%num_plgfx;
				}
			}
			l = 15;
			if (a > nl-365)  l = 15 - ((a - (nl-365))*15)/365;
			a = (a * 38) / 365;	// size
			if (l < 1) l = 1;
			ik_drsprite(screen, sm_stars[c].ds_x, sm_stars[c].ds_y, (c*64)&1023, a, spr_shockwave->spr[1], 5+(l<<8));

			if (sm_stars[c].novatype < 2 && player.num_ships>0 && (player.stardate-sm_stars[c].novadate) < nl-365 )
			{
				// nova kills enemies
				for (l = 0; l < STARMAP_MAX_FLEETS; l++)
				{
					if (sm_fleets[l].race != race_kawangi && sm_fleets[l].num_ships>0)
					{
						if (sm_fleets[l].system == c || starmap_stardist(sm_fleets[l].system, c) < player.stardate-sm_stars[c].novadate)
							sm_fleets[l].num_ships = 0;					
					}
				}

				if (player.system == c) // in system or trying to escape
				{
					if (player.enroute>0 && player.target != c)
					{
						if (player.enroute < player.stardate - sm_stars[c].novadate &&
							  player.enroute > player.stardate - sm_stars[c].novadate - 120)
						{
							player.num_ships = 0;
							player.death = 4;
						}
					}
					else if (player.stardate-sm_stars[c].novadate < 120)
					{	// kill if staying
						player.num_ships = 0;
						player.death = 4;
					}
				}
				else 
				{	// kill player
					l = get_distance(sm_stars[c].x - player.x, sm_stars[c].y - player.y);
					if (l > a/2+1-8 && l < a/2+1)
					{
						player.num_ships = 0;
						player.death = 4;
					}
				}	
			}

			a = 64+a/2;
			l = 15;
			if (player.stardate > sm_stars[c].novadate+365)
			{	
				l -= (player.stardate - (sm_stars[c].novadate+365)) >> 5;
				if (l < 0) l = 0;
			}
			if (l)
				ik_drsprite(screen, sm_stars[c].ds_x, sm_stars[c].ds_y, (c*64)&1023, a, spr_shockwave->spr[4], 5+(l<<8));
		}
		else if (sm_stars[c].novadate>0)	// && player.stardate >= sm_stars[c].novadate+nl)
		{
			sm_stars[c].novadate = 0;
		}
#endif
	}
	else if (sm_stars[c].color == -3)	// collapser effect
	{
		d = ti2 - sm_stars[c].novatime;
		if (d > 50)
			sm_stars[c].color = -2;
		a = d*2;
		l = 15; 
		if (d > 35) l-=d-35;
		if (l < 0) l=0;
		if (sm_stars[c].novatype == 2)	// main collapser
		{
			ik_drsprite(screen, sm_stars[c].ds_x, sm_stars[c].ds_y, (c*64)&1023, a*2, spr_shockwave->spr[2], 5+(l<<8));
		}
		else	// exploding stars
		{
			ik_drsprite(screen, sm_stars[c].ds_x, sm_stars[c].ds_y, (c*64)&1023, a, spr_shockwave->spr[3], 5+(l<<8));
		}

		a = d*2+30;
		l = 15;
		if (d > 15) l-=d-15;
		if (l>0)
			ik_drsprite(screen, sm_stars[c].ds_x, sm_stars[c].ds_y, (c*64)&1023, a, spr_shockwave->spr[4], 5+(l<<8));
	}


	for (c = 0; c < STARMAP_MAX_FLEETS; c++)
	{

#ifndef DEMO_VERSION
#ifdef STARMAP_DEBUGINFO
		if (sm_fleets[c].race == race_kawangi)
		{
			ik_print(screen, font_6x8, 176, 24, 3, "%d %d", sm_fleets[c].system, sm_fleets[c].target);
		}
#endif
#endif

		if (sm_fleets[c].explored>0 && (sm_fleets[c].num_ships>0 || (sm_fleets[c].blowtime>0 && sm_fleets[c].enroute>0)) )
		{
			l = sm_fleets[c].system;
			d = sm_fleets[c].target;
			if (sm_fleets[c].enroute)	// moving between stars (kawangi)
			{
				x = sm_stars[l].x + ((sm_stars[d].x-sm_stars[l].x)*sm_fleets[c].enroute)/sm_fleets[c].distance;
				y = sm_stars[l].y + ((sm_stars[d].y-sm_stars[l].y)*sm_fleets[c].enroute)/sm_fleets[c].distance;
				a = get_direction(sm_stars[d].x-sm_stars[l].x, sm_stars[d].y-sm_stars[l].y);
				
				if (sm_fleets[c].blowtime == 0)
				{
					ik_dspriteline(screen, cx+x+(sin1k[a]>>13), cy-y-(cos1k[a]>>13), sm_stars[d].ds_x, sm_stars[d].ds_y, 8, (t&15), 16, spr_IFtarget->spr[0], 4);

					if (sm_fleets[c].explored==2)
						ik_drsprite(screen, cx+x, cy-y, a, 24, hulls[shiptypes[sm_fleets[c].ships[0]].hull].sprite, 0);
					else
					{
						ik_drsprite(screen, cx+x, cy-y, a, 12, spr_IFtarget->spr[1], 4);
						ik_drsprite(screen, cx+x, cy-y, ((t*3+c*128)&1023), 24, spr_IFtarget->spr[9], 4);
					}
				}
				else
				{
					d = ti2 - sm_fleets[c].blowtime;
					if (d >= 100)
					{
						sm_fleets[c].blowtime = 0;
					}
					else
					{
						a = (spr_explode1->num * d) / 100;
						ik_drsprite(screen, cx+x, cy-y, (d*10)&1023, 24, spr_explode1->spr[a], 4);
						l = 15;
						if (d>40) l-=(d-40)/4;
						ik_drsprite(screen, cx+x, cy-y, (d*3)&1023, d, spr_shockwave->spr[0], 5+(l<<8));
					}
				}
			}
			else
			{
				if (sm_fleets[c].explored==2)
				{
					ik_drsprite(screen, 
											sm_stars[l].ds_x + (sin1k[(t*(c+3)+512) & 1023]>>12),
											sm_stars[l].ds_y - (cos1k[(t*(c+3)+512) & 1023]>>12),
											(t*(c+3) + 768)&1023,
											24,
											hulls[shiptypes[racefleets[races[sm_fleets[c].race].fleet].stype[0]].hull].sprite,
											0);
#ifdef STARMAP_DEBUGINFO
					ik_print(screen, font_6x8, sm_stars[l].ds_x + 8, sm_stars[l].ds_y - 16, 3, "f%d", c);
#endif
											//hulls[shiptypes[sm_fleets[c].ships[0]].hull].sprite, 0);
				}
				else if (sm_fleets[c].num_ships>0)
				{
					ik_drsprite(screen, 
											sm_stars[l].ds_x,
											sm_stars[l].ds_y,
											((t*3+c*128)&1023),
											32,
											spr_IFtarget->spr[9], 4);
#ifdef STARMAP_DEBUGINFO
					ik_print(screen, font_6x8, sm_stars[l].ds_x + 8, sm_stars[l].ds_y - 16, 3, "f%d", c);
#endif
				}
			}
		}
#ifdef STARMAP_DEBUGINFO
		else
		{
			l = sm_fleets[c].system;
			ik_drsprite(screen, 
									sm_stars[l].ds_x,
									sm_stars[l].ds_y,
									((t*3+c*128)&1023),
									32,
									spr_IFtarget->spr[9], 4);
		}
#endif
	}

	if (player.num_ships>0)
	{
		if (player.enroute)
		{
			c = player.system;
			l = player.target;
			if (player.fold)
			{	// fold
				a = rand()%32;

				if (t-player.hyptime < 96)
				{
					fs1 = new_sprite(64, 64);
					fs2 = new_sprite(64, 64);
					d = t-player.hyptime;
					for (y = 0; y < 64; y++)
					{
						if (y+d < 64)
							memcpy(fs1->data+y*64, ssp->data+(y+d)*64, 64);
						else
							memset(fs1->data+y*64, 0, 64);
						if (y < 20)
							for (x = 0; x < 64; x++)
							{
								if (fs1->data[y*64+x])
									fs1->data[y*64+x] = gfx_addbuffer[spr_weapons->spr[15]->data[y*32+((x+a)&31)]+(fs1->data[y*64+x]<<8)];
							}
						if (y+d-96 >= 0)
							memcpy(fs2->data+y*64, ssp->data+(y+d-96)*64, 64);
						else
							memset(fs2->data+y*64, 0, 64);
						if (y > 44)
							for (x = 0; x < 64; x++)
							{
								if (fs2->data[y*64+x])
									fs2->data[y*64+x] = gfx_addbuffer[spr_weapons->spr[15]->data[(63-y)*32+((x+a)&31)]+(fs2->data[y*64+x]<<8)];
							}

					}
					a = get_direction( sm_stars[l].x - sm_stars[c].x,
														 sm_stars[l].y - sm_stars[c].y);
					ik_drsprite(screen, 
											sm_stars[c].ds_x + (sin1k[a]>>12),
											sm_stars[c].ds_y - (cos1k[a]>>12),
											a,
											24,
											fs1, 0);
					ik_drsprite(screen, 
											sm_stars[l].ds_x - (sin1k[a]>>12),
											sm_stars[l].ds_y + (cos1k[a]>>12),
											a,
											24,
											fs2, 0);
					free_sprite(fs1);
					free_sprite(fs2);
				}
				else
				{
					a = get_direction( sm_stars[l].x - sm_stars[c].x,
														 sm_stars[l].y - sm_stars[c].y);
					ik_drsprite(screen, 
											sm_stars[l].ds_x - (sin1k[a]>>12),
											sm_stars[l].ds_y + (cos1k[a]>>12),
											a,
											24,
											ssp, 0);
				}
			}	
			else if (shiptypes[0].engine>-1 && shipsystems[shiptypes[0].engine].par[0]==666)
			{	// hyper
				a = get_direction( sm_stars[l].x - sm_stars[c].x,
													 sm_stars[l].y - sm_stars[c].y);
				if (t-player.hyptime < 32)
				{
					if (t-player.hyptime < 16)
						ik_drsprite(screen, 
												sm_stars[c].ds_x + (sin1k[a]>>12),
												sm_stars[c].ds_y - (cos1k[a]>>12),
												a,
												(16-(t-player.hyptime))*24/16,
												ssp, 0);
					d = 15-((t-player.hyptime)*14)/32;
					ik_drsprite(screen, 
											sm_stars[c].ds_x + (sin1k[a]>>12),
											sm_stars[c].ds_y - (cos1k[a]>>12),
											a,
											64,
											spr_shockwave->spr[4], 5+(d<<8));
				}
				else if (t-player.hyptime > 75 && t-player.hyptime < 107)
				{
					/*if (t-player.hyptime < 91)
						ik_drsprite(screen, 
												sm_stars[l].ds_x - (sin1k[a]>>12),
												sm_stars[l].ds_y + (cos1k[a]>>12),
												a,
												((t-player.hyptime)-75)*24/16,
												spr_ships->spr[2], 0);
					else*/
					ik_drsprite(screen, 
											sm_stars[l].ds_x - (sin1k[a]>>12),
											sm_stars[l].ds_y + (cos1k[a]>>12),
											a,
											24,
											ssp, 0);

					d = 15-((t-player.hyptime-75)*14)/32;
					ik_drsprite(screen, 
											sm_stars[l].ds_x - (sin1k[a]>>12),
											sm_stars[l].ds_y + (cos1k[a]>>12),
											a,
											64,
											spr_shockwave->spr[4], 5+(d<<8));
				}
				else if (t-player.hyptime >= 107)
				{
					ik_drsprite(screen, 
											sm_stars[l].ds_x - (sin1k[a]>>12),
											sm_stars[l].ds_y + (cos1k[a]>>12),
											a,
											24,
											ssp, 0);
				}
			}
			else
			{
				a = player.enroute;
				if (a < 640)
					a = 128+(((a+384) * (a+384))>>11);
				if (a > player.distance - 640)
					a = player.distance-128-(((player.distance+384-a) * (player.distance+384-a) )>>11);

				d = MAX( (abs(sm_stars[l].x-sm_stars[c].x)), (abs(sm_stars[l].y-sm_stars[c].y)) );
				a = (a * d) / player.distance;
				x = (sm_stars[c].x * (d-a) +
						sm_stars[l].x * a) / d;
				y = (sm_stars[c].y * (d-a) +
						sm_stars[l].y * a) / d;
				a = get_direction( sm_stars[l].x - sm_stars[c].x,
													 sm_stars[l].y - sm_stars[c].y);
				player.a = a;
				ik_drsprite(screen, 
										cx+x,
										cy-y,
										a,
										24,
										ssp, 0);
				a = (player.enroute * d)/player.distance;
				player.x = (sm_stars[c].x * (d-a) +
										sm_stars[l].x * a) / d;
				player.y = (sm_stars[c].y * (d-a) +
										sm_stars[l].y * a) / d;
			}

			if (player.explore)
			{
				l = 11+4*((t&31)>24);
				l = 5 + (l<<8);
				a = get_direction( sm_stars[player.target].x - x,
													 sm_stars[player.target].y - y);

				ik_dspriteline(screen, 
											cx + x + (sin1k[a]>>12),
											cy - y - (cos1k[a]>>12),
											sm_stars[player.target].ds_x - ((sin1k[a]*3)>>13),
											sm_stars[player.target].ds_y + ((cos1k[a]*3)>>13),
											8, (t&15), 16, spr_IFtarget->spr[4], l);
				ik_drsprite(screen, 
										sm_stars[player.target].ds_x - (sin1k[a]>>12),
										sm_stars[player.target].ds_y + (cos1k[a]>>12),
										a,
										12,
										spr_IFtarget->spr[5], l);
				ik_drsprite(screen, 
										cx+sm_holes[player.explore-1].x,
										cy-sm_holes[player.explore-1].y,
										1023-((t*8)&1023),
										24,
										spr_IFtarget->spr[6], 0);
			}
		}

		if (player.system>-1 && !player.enroute)
		{
			c = player.system;
			player.x = sm_stars[c].x;
			player.y = sm_stars[c].y;
			if (player.target==-1 || player.target==player.system) // in orbit
			{
				ik_drsprite(screen, 
										sm_stars[c].ds_x + (sin1k[(t*8) & 1023]>>12),
										sm_stars[c].ds_y - (cos1k[(t*8) & 1023]>>12),
										(t*8 + 256)&1023,
										24,
										ssp, 0);
				// hire and hunt buttons
				if (player.target == player.system && sm_stars[player.target].explored==2)
				{
					l = sm_stars[player.target].card;
					if (ecards[l].type == card_ally)
					{
						ik_drsprite(screen, 
												sm_stars[player.target].ds_x,
												sm_stars[player.target].ds_y + 21,
												0,
												32,
												spr_IFbutton->spr[20], 1+((11+4*((t&31)>24))<<8));
					}
					else if (ecards[l].type == card_lifeform)
					{
						ik_drsprite(screen, 
												sm_stars[player.target].ds_x,
												sm_stars[player.target].ds_y + 21,
												0,
												32,
												spr_IFbutton->spr[21], 1+((11+4*((t&31)>24))<<8));
					}
				}
			}
			else	// targeted
			{
				l = 11+4*((t&31)>24);
				l = 5 + (l<<8);
				a = get_direction( sm_stars[player.target].x - sm_stars[c].x,
													 sm_stars[player.target].y - sm_stars[c].y);
				bu[0] = 0; nbu = 1;
				if (player.distance <= 2380)
				{
					for (x = 0; x < player.num_ships; x++)
						if (shiptypes[player.ships[x]].flag & 4)	// kuti
							bu[nbu++] = 1;
					if (player.target>-1 && sm_stars[player.target].explored==0)
						for (x = 0; x < shiptypes[0].num_systems; x++)
							if (shipsystems[shiptypes[0].system[x]].type == sys_misc && shipsystems[shiptypes[0].system[x]].par[0]==0)
								bu[nbu++] = 2;
				}

				ik_dspriteline(screen, 
											sm_stars[c].ds_x + (sin1k[a]>>12),
											sm_stars[c].ds_y - (cos1k[a]>>12),
											sm_stars[player.target].ds_x - ((sin1k[a]*3)>>13),
											sm_stars[player.target].ds_y + ((cos1k[a]*3)>>13),
										8, (t&15), 16, spr_IFtarget->spr[4], l);
				ik_drsprite(screen, 
										sm_stars[player.target].ds_x - (sin1k[a]>>12),
										sm_stars[player.target].ds_y + (cos1k[a]>>12),
										a,
										12,
										spr_IFtarget->spr[5], l);

				// draw engage / fold buttons
				x = (sm_stars[player.target].y>=sm_stars[c].y)-(sm_stars[player.target].y<sm_stars[c].y);
				for (d = 0; d < nbu; d++)
				{
					y = sm_stars[player.target].ds_y - x * (12+nbu*5) - nbu*5 + d*10;
					ik_drsprite(screen, 
											sm_stars[player.target].ds_x,
											y + 9,
											0,
											32,
											spr_IFbutton->spr[12+bu[d]], 1+((11+4*((t&31)>24))<<8));
				}
				ik_drsprite(screen, 
										sm_stars[c].ds_x + (sin1k[a]>>12),
										sm_stars[c].ds_y - (cos1k[a]>>12),
										a,
										24,
										ssp, 0);

				ik_print(screen, font_6x8,
								(sm_stars[c].ds_x+sm_stars[player.target].ds_x)>>1,
								(sm_stars[c].ds_y+sm_stars[player.target].ds_y)>>1,
								0, textstring[STR_STARMAP_LYEARS], player.distance/365, ((player.distance%365)*100)/365);
	//							0, "%d.%02d LY", player.distance/256, ((player.distance&255)*100)>>8);
				if (shiptypes[0].engine>-1 && shiptypes[0].sysdmg[shiptypes[0].sys_eng]==0)
				{
					sp1 = shipsystems[shiptypes[0].engine].par[0];
					sp2 = shipsystems[shiptypes[0].engine].par[1];
				}
				else if (shiptypes[0].thrust>-1 && shiptypes[0].sysdmg[shiptypes[0].sys_thru]==0)
				{
					sp1 = sp2 = 1;
				}
				else
				{ sp1 = sp2 = 0; }
				if (sp1 == 666)	// hyperdrive
				{	
					if (player.stardate-player.hypdate > 60)
						a = 0;
					else
						a = 60-(player.stardate-player.hypdate);
				}
				else if (sp1 > 0 && sp2 > 0)
					a = ((player.distance*(256-player.nebula)*256)/sp1 + (player.distance*player.nebula*256)/sp2)>>16;
				else
					a = player.distance*4;
	//			a = ((player.distance*(256-player.nebula)*365)/sp1 + (player.distance*player.nebula*365)/sp2)>>16;
				ik_print(screen, font_6x8,
								(sm_stars[c].ds_x+sm_stars[player.target].ds_x)>>1,
								(sm_stars[c].ds_y+sm_stars[player.target].ds_y+16)>>1,
								0, textstring[STR_STARMAP_NDAYS], a);
			}
			if (player.target > -1)
			{
				ik_drsprite(screen, 
										sm_stars[player.target].ds_x,
										sm_stars[player.target].ds_y,
										1023-((t*8)&1023),
										24,
										spr_IFtarget->spr[8], 0);
			}
		}
	}
	else
	{
		d = ti2 - player.deatht;
		if (player.death == 2)
		{
			if (d < 150)
			{
				a = ((65536-cos1k[d*256/150])>>3)&1023;
				x = ((player.x-sm_holes[player.hole].x)*cos1k[a] + 
						(player.y-sm_holes[player.hole].y)*sin1k[a]) >> 16;
				y = ((player.y-sm_holes[player.hole].y)*cos1k[a] - 
						(player.x-sm_holes[player.hole].x)*sin1k[a]) >> 16;
				l = 1024*(150-d)/150;
				ik_drsprite(screen, 
										cx+sm_holes[player.hole].x+((x*l)>>10),
										cy-sm_holes[player.hole].y-((y*l)>>10),
										(player.a+a)&1023,
										(24*l)>>10,
										ssp, 0);

			}
		}
		else		// explode ship
		{
			if (d<100)
			{
				if (player.enroute)
				{ x = cx + player.x; y = cy - player.y; }
				else
				{
					x = sm_stars[player.system].ds_x + (sin1k[(player.deatht*8) & 1023]>>12);
					y = sm_stars[player.system].ds_y - (cos1k[(player.deatht*8) & 1023]>>12);
				}
				a = (spr_explode1->num * d) / 100;
				ik_drsprite(screen, x, y, (d*10)&1023, 24, spr_explode1->spr[a], 4);
				l = 15;
				if (d>40) l-=(d-40)/4;
				ik_drsprite(screen, x, y, (d*3)&1023, d, spr_shockwave->spr[0], 5+(l<<8));
			}
		}
	}

	ik_setclip(0,0,640,480);
	l = 0; a=player.stardate%365;
	for (c = 0; c < 12; c++)
		if (a >= months[c].sd)
			l = c;
	a = a + 1 - months[l].sd;
//	sprintf(lne, "%s", player.captname, 60-strlen(player.captname));

	if (!settings.opt_timeremaining || player.stardate>=10*365)
		sprintf(cal, textstring[STR_STARMAP_DATE], a, months[l].name, player.stardate/365+4580);
	else
	{
		c = 10*365 - player.stardate;
		sprintf(cal, textstring[STR_STARMAP_DAYSLEFT], c);
	}

	sprintf(lne, textstring[STR_STARMAP_CAPTAIN], player.captname, 66-strlen(player.captname));
	sprintf(top, lne, cal);
	interface_drawborder(screen,
											 SM_MAP_X, SM_MAP_Y, SM_MAP_X + 480, SM_MAP_Y + 480,
											 0, STARMAP_INTERFACE_COLOR, top);

	// draw selected ship
	starmap_displayship(t, player.ships[player.sel_ship]);

	// draw inventory
	if (player.num_items > 12)
		hud.invslider = MIN(hud.invslider, player.num_items-12);

	sprintf(top, textstring[STR_STARMAP_CARGO]);
	interface_drawborder(screen,
											 SM_INV_X, SM_INV_Y, SM_INV_X + 160, SM_INV_Y + 128,
											 1, STARMAP_INTERFACE_COLOR, top);
	for (c = 0; c < player.num_items; c++)
	{
		if (c >= hud.invslider && c < hud.invslider + 12)
		{
			ik_print(screen, font_4x8, SM_INV_X + 24, SM_INV_Y + 24 + (c-hud.invslider) * 8, 0*(hud.invselect==c), itemtypes[player.items[c]].name);
			if (!player.itemflags[c])
			{
				l = itemtypes[player.items[c] & 255].type;
				if ((l == item_weapon) || (l == item_system))
					ik_dsprite(screen, SM_INV_X + 16, SM_INV_Y + 24 + (c-hud.invslider)*8, spr_IFarrows->spr[13], 2+(3<<8));
				else if (l == item_device)
					ik_dsprite(screen, SM_INV_X + 16, SM_INV_Y + 24 + (c-hud.invslider)*8, spr_IFarrows->spr[14], 2+(3<<8));
			}
			else	// broken
			{
				ik_dsprite(screen, SM_INV_X + 16, SM_INV_Y + 24 + (c-hud.invslider)*8, spr_IFarrows->spr[15], 2+(3<<8));
			}
//		ik_print(screen, font_6x8, SM_INV_X + 16, SM_INV_Y + 24 + c * 8, 0, "%d", player.items[c]);
		}
	}
	if (player.num_items > 12)
	{
		interface_drawslider(screen, SM_INV_X + 136, SM_INV_Y + 32, 1, 80, player.num_items-12, hud.invslider, STARMAP_INTERFACE_COLOR);
		ik_dsprite(screen, SM_INV_X + 136, SM_INV_Y + 24, spr_IFarrows->spr[5], 2+(STARMAP_INTERFACE_COLOR<<8));
		ik_dsprite(screen, SM_INV_X + 136, SM_INV_Y + 112, spr_IFarrows->spr[4], 2+(STARMAP_INTERFACE_COLOR<<8));
	}
	l = STARMAP_INTERFACE_COLOR * (hud.invselect>-1);
	if (hud.invselect > -1)
		c = itemtypes[player.items[hud.invselect]].type;

	/*
	ik_dsprite(screen, SM_INV_X + 16, SM_INV_Y + 120, spr_IFbutton->spr[11], 2+(l<<8));
	if (hud.invselect > -1)
		l = STARMAP_INTERFACE_COLOR * (c == item_device);
	else
		l = 0;
	ik_dsprite(screen, SM_INV_X + 48, SM_INV_Y + 120, spr_IFbutton->spr[12], 2+(l<<8));

	if (hud.invselect > -1)
		l = STARMAP_INTERFACE_COLOR * ((c == item_weapon) || (c == item_system));
	else
		l = 0;

	ik_dsprite(screen, SM_INV_X + 80, SM_INV_Y + 120, spr_IFbutton->spr[13], 2+(l<<8));
	if (hud.invselect > -1)
		l = STARMAP_INTERFACE_COLOR * ((player.itemflags[hud.invselect]&item_broken)>0);
	else
		l = 0;
	ik_dsprite(screen, SM_INV_X + 112, SM_INV_Y + 120, spr_IFbutton->spr[14], 2+(l<<8));
	*/
	// draw selection (system) info
	sprintf(top, textstring[STR_STARMAP_SELECT]);
	if (player.target > -1)
	{
		if (sm_stars[player.target].explored)
			sprintf(top, sm_stars[player.target].planetname);
		else
			sprintf(top, sm_stars[player.target].starname);
	}
	interface_drawborder(screen,
											 SM_SEL_X, SM_SEL_Y, SM_SEL_X + 160, SM_SEL_Y + 96,
											 1, STARMAP_INTERFACE_COLOR, top);
	if (player.target > -1)
	{
		if (sm_stars[player.target].explored) // planet info
		{
			ik_dsprite(screen, SM_SEL_X + 16, SM_SEL_Y + 24, spr_SMplanet2->spr[sm_stars[player.target].planetgfx], 0);
			interface_textbox(screen, font_4x8,
												SM_SEL_X + 84, SM_SEL_Y + 24, 64, 64, 0,
												platypes[sm_stars[player.target].planet].text);
		}
		else if (sm_stars[player.target].color >= 0) // star info
		{
			ik_dsprite(screen, SM_SEL_X + 16, SM_SEL_Y + 24, spr_SMstars2->spr[sm_stars[player.target].color], 0);
			interface_textbox(screen, font_4x8,
												SM_SEL_X + 84, SM_SEL_Y + 24, 64, 64, 0,
												startypes[sm_stars[player.target].color].text);
		}
		else
		{
			ik_dsprite(screen, SM_SEL_X + 16, SM_SEL_Y + 24, spr_SMplanet2->spr[22], 0);
			interface_textbox(screen, font_4x8,
												SM_SEL_X + 84, SM_SEL_Y + 24, 64, 64, 0,
												platypes[10].text);
		}
		ik_dsprite(screen, SM_SEL_X + 16, SM_SEL_Y + 24, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
	}

}

int starmap_findstar(int32 mx, int32 my)
{
	int c;

	for (c = 0; c < num_stars; c++)
	if (sm_stars[c].color>-1)
	{
		if (mx > sm_stars[c].ds_x - 8 && mx < sm_stars[c].ds_x + 8 &&
				my > sm_stars[c].ds_y - 8 && my < sm_stars[c].ds_y + 8)
			return c;
	}
	return -1;
}

int starmap_stardist(int32 s1, int32 s2)
{
	int r;

	if (s1==s2)
		return 0;

	r = (int32)(sqrt((sm_stars[s2].x-sm_stars[s1].x) * (sm_stars[s2].x-sm_stars[s1].x) +
									(sm_stars[s2].y-sm_stars[s1].y) * (sm_stars[s2].y-sm_stars[s1].y)) *
									3.26*365/64);	// "light days"

	return r;
}

int starmap_nebuladist(int32 s1, int32 s2)
{
	int x, y;
	int r, d, l;
	int dx, dy;

	if (s1 == s2)
		return 0;

	dx = sm_stars[s2].x-sm_stars[s1].x;
	dy = sm_stars[s2].y-sm_stars[s1].y;
	l = MAX(ABS(dx), ABS(dy));

	r = 0; d = l;
	while (d--)
	{
		x = (sm_stars[s1].x*(l-d) + sm_stars[s2].x*(d))/l;
		y = (sm_stars[s1].y*(l-d) + sm_stars[s2].y*(d))/l;
		r+= (sm_nebulamap[((240-y)<<9)+(240+x)]>0);
	}

	r = (r*256)/l;

	return r;
}


void starmap_removeship(int32 n)
{
	int32 c;

	if (player.sel_ship == n)
		player.sel_ship = 0;
	else if (player.sel_ship > n)
		player.sel_ship--;
	for (c = n; c < player.num_ships-1; c++)
	{
		player.ships[c] = player.ships[c+1];
	}
	player.num_ships--;
}

void killstar(int32 c)
{
	int32 n;

	sm_stars[c].color = -2;
	for (n = 0; n < STARMAP_MAX_FLEETS; n++)
	if (sm_fleets[n].num_ships > 0)
	{
		if (sm_stars[sm_fleets[n].system].color < 0)
			sm_fleets[n].num_ships = 0;
	}
}


void starmap_advancedays(int32 n)
{
	int c;
	int s, sy;
#ifndef DEMO_VERSION
	int f;
#endif

	starmap_sensefleets();

	for (c = 0; c < n; c++)
	{
		player.stardate++;
		for (s = 0; s < player.num_ships; s++)
		{
			if (shiptypes[player.ships[s]].hits < hulls[shiptypes[player.ships[s]].hull].hits*256)
			{
				shiptypes[player.ships[s]].hits += 128;
				for (sy = 0; sy < shiptypes[player.ships[s]].num_systems; sy++)
					if (shipsystems[shiptypes[player.ships[s]].system[sy]].type == sys_damage)
						shiptypes[player.ships[s]].hits = hulls[shiptypes[player.ships[s]].hull].hits*256;
				if (shiptypes[player.ships[s]].hits >= hulls[shiptypes[player.ships[s]].hull].hits*256)
				{	
					ik_print_log("Finished hull repairs on the %s.\n", shiptypes[player.ships[s]].name);
					shiptypes[player.ships[s]].hits = hulls[shiptypes[player.ships[s]].hull].hits*256;
				}
			}
		}

		if (settings.opt_timerwarnings)
		{
			if (player.stardate == 8*365)
			{
				timer_warning = 1;
			}
			else if (player.stardate == 10*365)
			{
				timer_warning = 2;
			}
		}

#ifndef DEMO_VERSION
		for (s = 0; s < STARMAP_MAX_FLEETS; s++)
		if (sm_fleets[s].num_ships > 0 && sm_fleets[s].enroute > 0)
		{	// kawangi moves
			sm_fleets[s].enroute += 4;
			if (sm_fleets[s].enroute >= sm_fleets[s].distance)
			{	// kawangi enters system
				sm_fleets[s].system = sm_fleets[s].target;
				sm_fleets[s].enroute = 0;
				if (sm_fleets[s].system != homesystem)
					sm_fleets[s].distance = player.stardate + 365;
				else
					sm_fleets[s].distance = 0;

				if (sm_fleets[s].system == player.system && player.enroute == 0) // meets player
				{
					kawangi_incoming = 1;
				}

				// kill any other fleets at system
				for (f = 0; f < STARMAP_MAX_FLEETS; f++)
				if (s != f && sm_fleets[f].num_ships > 0 && sm_fleets[f].system == sm_fleets[s].system)
				{
					sm_fleets[f].num_ships = 0;
				}

			}
		}
		else if (sm_fleets[s].race == race_kawangi && sm_fleets[s].num_ships > 0)
		{
			if (player.stardate > sm_fleets[s].distance && sm_fleets[s].distance > 0)
			{
				starmap_kawangimove(s);
			}
		}
#endif

	}
}

void starmap_sensefleets()
{
	int c, r;
	int x, y;

	for (c = 0; c < STARMAP_MAX_FLEETS; c++)
	if (sm_fleets[c].explored == 0 && sm_fleets[c].num_ships > 0)
	{
		if (sm_fleets[c].enroute == 0)
			r = get_distance ( sm_stars[sm_fleets[c].system].x - player.x, sm_stars[sm_fleets[c].system].y - player.y);
		else
		{
			x = sm_stars[sm_fleets[c].system].x + ((sm_stars[sm_fleets[c].target].x-sm_stars[sm_fleets[c].system].x)*sm_fleets[c].enroute)/sm_fleets[c].distance;
			y = sm_stars[sm_fleets[c].system].y + ((sm_stars[sm_fleets[c].target].y-sm_stars[sm_fleets[c].system].y)*sm_fleets[c].enroute)/sm_fleets[c].distance;

			r = get_distance ( x - player.x, y - player.y);
			if (sm_fleets[c].explored == 1)
				sm_fleets[c].explored = 0;
		}
		if (r < shiptypes[0].sensor)
		{
			sm_fleets[c].explored = 1;
		}
	}
	else if (sm_fleets[c].explored == 1 && sm_fleets[c].num_ships > 0)
	{
		if (sm_fleets[c].enroute)
		{
			x = sm_stars[sm_fleets[c].system].x + ((sm_stars[sm_fleets[c].target].x-sm_stars[sm_fleets[c].system].x)*sm_fleets[c].enroute)/sm_fleets[c].distance;
			y = sm_stars[sm_fleets[c].system].y + ((sm_stars[sm_fleets[c].target].y-sm_stars[sm_fleets[c].system].y)*sm_fleets[c].enroute)/sm_fleets[c].distance;

			r = get_distance ( x - player.x, y - player.y);
			if (r > shiptypes[0].sensor)
				sm_fleets[c].explored = 0;
		}
	}
}


void starmap_flee()
{
	int32 c;
	int32 r;
	int32 d;
	int32 t;
	int32 mexp, dexp;
	int32 msim, dsim;
	int32 mdis, ddis;

	d = -1; 

	dsim = ddis = dexp = -1;
	msim = mdis = mexp = 12000;

	for (c = 0; c < num_stars; c++)
	{
		if (sm_stars[c].color >= 0 && c!=homesystem && c!=player.system)
		{
			// check simulated move
			r = simulate_move(c);
			if (r > -1 && r < msim)
			{
				dsim = c; msim = r;
			}
			// check simulated move to explored star
			if (r > -1 && r < mexp && sm_stars[c].explored==2)
			{
				dexp = c; mexp = r;
			}

			// check distance
			r = get_distance( sm_stars[c].x - player.x, sm_stars[c].y - player.y);
			if (r > -1 && r < mdis)
			{
				ddis = c; mdis = r;
			}
		}
	}

	if (dexp > -1)
	{
		d = dexp;
		if (mexp > -1 && msim < mexp/2)
			d = dsim;
	}
	else if (dsim > -1)
	{
		d = dsim;
	}
	else
		d = ddis;

	if (d > -1)
	{
		player.fold = 0;
		c = starmap_stardist(player.system, d); //get_distance( sm_stars[d].x - player.x, sm_stars[d].y - player.y ) ;
		if (c <= 2380)
		{
			for (c = 0; c < player.num_ships; c++)
				if (shiptypes[player.ships[c]].flag & 4)	// kuti
					player.fold = 1;
		}
		
		t = get_ik_timer(0);
		if (shipsystems[shiptypes[0].engine].par[0]==666 || player.fold)
			player.hyptime = t;
		player.target = d;
		player.engage = 2;
		player.distance = starmap_stardist(player.system, player.target);
		player.nebula = starmap_nebuladist(player.system, player.target);
	}

}

int32 simulate_move(int32 star)
{
	int32 dt = player.stardate;
	int32 sp1, sp2;
	int32 s, d;
	int32 str = player.system, dst = star;
	int32 a,c;
	int32 x,y;
	int32 end;
	int32 fold = 0;

	d = starmap_stardist(str, dst);
	s = 1;

	if (star == player.system)
		return -1;

//	if (sm_stars[star].novadate>0)
//		return -1;

	// don't flee into an enemy fleet
	for (x = 0; x < STARMAP_MAX_FLEETS; x++)
	{
		if (sm_fleets[x].explored>0 && sm_fleets[x].race!=race_klakar && sm_fleets[x].num_ships>0)
		{
			if (sm_fleets[x].system == star)
				return -1;
		}
	}

	if (d <= 2380)
	{
		for (c = 0; c < player.num_ships; c++)
			if (shiptypes[player.ships[c]].flag & 4)	// kuti 
				fold = 1;
	}

	if (shiptypes[0].engine > -1 && shiptypes[0].sysdmg[shiptypes[0].sys_eng]==0)
	{
		sp1 = shipsystems[shiptypes[0].engine].par[0];
		sp2 = shipsystems[shiptypes[0].engine].par[1];
	}
	else if (shiptypes[0].thrust > -1 && shiptypes[0].sysdmg[shiptypes[0].sys_thru]==0)
	{
		sp1 = 1; sp2 = 1;
	}
	else
	{
		sp1 = 0; sp2 = 0;
	}
	if (fold)	// fold
	{
		if (player.stardate - player.foldate < 7)
			dt += 7 - (player.stardate - player.foldate);
		s = d;
	}
	else if (sp1 == 666)		// hyperdrive
	{
		if (player.stardate - player.hypdate < 60)
			dt += 60 - (player.stardate - player.hypdate);
		s = d;
	}

	if (s==d)
	{	// check for deaths by nova
		for (c = 0; c < num_stars; c++)
		if (sm_stars[c].novadate>0 && dt>sm_stars[c].novadate && dt<sm_stars[c].novadate+4*365)
		{
			a = ((dt-sm_stars[c].novadate) * 39) / 365;	// size
			if (get_distance(sm_stars[c].x - sm_stars[dst].x, sm_stars[c].y - sm_stars[dst].y) < a/2+1)
			{	
				return -1;	
			}
		}
		return dt-player.stardate;
	}

	end = 0;
	while (!end && !must_quit)
	{
		ik_eventhandler();

		x = (sm_stars[str].x * (d-s) +
				 sm_stars[dst].x * s) / d;
		y = (sm_stars[str].y * (d-s) +
				 sm_stars[dst].y * s) / d;
		if (sp1 > 0 && sp2 > 0)
		{
			if (sm_nebulamap[((240-y)<<9)+(240+x)])
				s += sp2*2;
			else
				s += sp1*2;
			dt+=2;
		}
		else
		{
			s += 1;
			dt+=4;
		}

		// hit black holes?
		for (c = 0; c < num_holes; c++)
		if (sm_holes[c].size>0)
		{
			a = get_distance(sm_holes[c].x - x, sm_holes[c].y - y);
			if (sp1>0)
			{
				if (a < 96/(MAX(sp1,6)) )
				{
					end = 1;
				}
			}
			else if (a < 96/6)
			{
				end = 1;
			}
		}

		// hit novas
		for (c = 0; c < num_stars; c++)
		if (sm_stars[c].novadate>0 && dt>sm_stars[c].novadate && dt<sm_stars[c].novadate+4*365)
		{
			a = ((dt-sm_stars[c].novadate) * 39) / 365;	// size
			if (get_distance(sm_stars[c].x - x, sm_stars[c].y - y) < a/2+1)
			{	
				end = 1;
			}
		}

		if (s >= d)
			end = 2;
	}

	if (end==1)
		return -1;

	return dt - player.stardate;
}


void help_screen()
{
	int32 end;
	int32 c, mc;
	int32 t;
	int32 x, y;
	t_ik_image *bg;

	bg = ik_load_pcx("graphics/help.pcx", NULL);

	prep_screen();
	ik_copybox(bg, screen, 0, 0, 640, 480, 0,0);

#ifdef STARMAP_BUILD_HELP
	x = 556; y = 404;
	interface_thinborder(screen, x, y+4, x+80, y+72, STARMAP_INTERFACE_COLOR, 2+STARMAP_INTERFACE_COLOR*16);
	ik_print(screen, font_6x8, x+4, y+=8, 3, "MAGNIFIER");
	interface_textbox(screen, font_6x8, x+4, y+=8, 72, 64, 0, "If you can't read small text, toggle 2X magnifier by pressing F2 or CTRL."); 

	x = 16; y = 228;
	interface_thinborder(screen, x, y+4, x+128, y+64, STARMAP_INTERFACE_COLOR, 2+STARMAP_INTERFACE_COLOR*16);
	ik_print(screen, font_6x8, x+4, y+=8, 11, "NOTE ON STAR DRIVES");
	interface_textbox(screen, font_4x8, x+4, y+=8, 112, 64, 0, "In an emergency or if you have no star drive, you can slowly travel between stars using your thrusters at roughly the speed of light."); 


	interface_thinborder(screen, 240, 176, 400, 296, STARMAP_INTERFACE_COLOR);

	interface_thinborder(screen, 248, 16, 352, 68, STARMAP_INTERFACE_COLOR, 2+STARMAP_INTERFACE_COLOR*16);
	y = 12;

	ik_print(screen, font_6x8, 252, y+=8, 11, "CURRENT DATE");
	interface_textbox(screen, font_4x8, 252, y+=8, 96, 64, 0, "Click on the day, month or year to advance date, or wait at the current location.");

	interface_thinborder(screen, 376, 56, 456, 116, STARMAP_INTERFACE_COLOR, 2+STARMAP_INTERFACE_COLOR*16);
	y = 52;
	ik_print(screen, font_6x8, 380, y+=8, 11, "NEBULA");
	interface_textbox(screen, font_4x8, 380, y+=8, 72, 64, 0, "Traveling through these clouds of dust and gas is very slow without a special drive.");

	x = 408; y = 116;
	interface_thinborder(screen, x, y+4, x+96, y+88, STARMAP_INTERFACE_COLOR, 2+STARMAP_INTERFACE_COLOR*16);
	ik_print(screen, font_6x8, x+4, y+=8, 11, "STAR LANE");
	interface_textbox(screen, font_4x8, x+4, y+=8, 88, 96, 0, "Once you select a star system by clicking it, a star lane is displayed along with distance and estimated travel time. Press the ENGAGE button to move between the stars.");

	x = 512; y = 132;
	interface_thinborder(screen, x, y+4, x+72, y+56, STARMAP_INTERFACE_COLOR, 2+STARMAP_INTERFACE_COLOR*16);
	ik_print(screen, font_6x8, x+4, y+=8, 11, "UNEXPLORED");
	interface_textbox(screen, font_4x8, x+4, y+=8, 68, 32, 0, "This star hasn't been visited yet. No planets are displayed.");

	x = 440; y = 204;
	interface_thinborder(screen, x, y+4, x+144, y+32, STARMAP_INTERFACE_COLOR, 2+STARMAP_INTERFACE_COLOR*16);
	ik_print(screen, font_6x8, x+4, y+=8, 11, "BLACK HOLE");
	interface_textbox(screen, font_4x8, x+4, y+=8, 136, 32, 0, "Dangerous ship-eating singularity."); 

	x = 440; y = 244;
	interface_thinborder(screen, x, y+4, x+144, y+56, STARMAP_INTERFACE_COLOR, 2+STARMAP_INTERFACE_COLOR*16);
	ik_print(screen, font_6x8, x+4, y+=8, 11, "YOUR STARSHIP IN ORBIT");
	interface_textbox(screen, font_4x8, x+4, y+=8, 136, 64, 0, "When no star lane has been set, your ship is shown orbiting the star in your current system, along with any planets you have found."); 

	x = 440; y = 308;
	interface_thinborder(screen, x, y+4, x+144, y+48, STARMAP_INTERFACE_COLOR, 2+STARMAP_INTERFACE_COLOR*16);
	ik_print(screen, font_6x8, x+4, y+=8, 11, "ALIEN SPACECRAFT");
	interface_textbox(screen, font_4x8, x+4, y+=8, 136, 64, 0, "When you discover an alien spacecraft, it is displayed in orbit within the star system."); 

	x = 248; y = 332;
	interface_thinborder(screen, x, y+4, x+184, y+56, STARMAP_INTERFACE_COLOR, 2+STARMAP_INTERFACE_COLOR*16);
	ik_print(screen, font_6x8, x+4, y+=8, 11, "STAR SYSTEM SELECTION");
	interface_textbox(screen, font_4x8, x+4, y+=8, 176, 64, 0, "This window describes the currently selected star system. If the system has already been explored, information about the planet you found there is shown instead."); 

	x = 64; y = 292;
	interface_thinborder(screen, x, y+4, x+160, y+64, STARMAP_INTERFACE_COLOR, 2+STARMAP_INTERFACE_COLOR*16);
	ik_print(screen, font_6x8, x+4, y+=8, 11, "CARGO LISTING");
	y+=8*interface_textbox(screen, font_4x8, x+4, y+=8, 152, 64, 0, "A list of all the items you have on board. Many can be used in some way."); 

	ik_print(screen, font_4x8, x+4, y+=8, 0, "Click    to activate a device.");
	ik_dsprite(screen, x+28, y, spr_IFarrows->spr[14], 2+(3<<8));
	ik_print(screen, font_4x8, x+4, y+=8, 0, "Click    to install a ship system.");
	ik_dsprite(screen, x+28, y, spr_IFarrows->spr[13], 2+(3<<8));

	interface_thinborder(screen, 144, 48, 232, 228, STARMAP_INTERFACE_COLOR, 2+STARMAP_INTERFACE_COLOR*16);

	y = 44;
	ik_print(screen, font_6x8, 148, y+=8, 11, "ALLIED SHIPS");
	ik_copybox(screen, screen, 16, 24, 48, 40, 148, (y+=8)-1);
	ik_print(screen, font_4x8, 148, (y+=16), 0, "Click to select ship.");

	y+=6;
	ik_print(screen, font_6x8, 148, y+=8, 11, "HULL DAMAGE");
	ik_copybox(screen, screen, 80, 24, 144, 40, 148, y+=8);
	ik_print(screen, font_4x8, 148, y+=16, 0, "Click to repair.");

	y+=6;
	ik_print(screen, font_6x8, 148, y+=8, 11, "SHIP SYSTEMS");
	ik_dsprite(screen, 142, (y+=8)-5, spr_IFsystem->spr[1], 2+(1<<8));
	ik_print(screen, font_4x8, 156, y, 1, "Weapons");
	ik_dsprite(screen, 142, (y+=8)-5, spr_IFsystem->spr[5], 2+(3<<8));
	ik_print(screen, font_4x8, 156, y, 3, "Star Drive");
	ik_dsprite(screen, 142, (y+=8)-5, spr_IFsystem->spr[9], 2+(2<<8));
	ik_print(screen, font_4x8, 156, y, 2, "Combat Thrusters");
	ik_print(screen, font_4x8, 156, y+=8, 5, "Other Systems");

	y+=6;
	ik_print(screen, font_6x8, 148, y+=8, 11, "SYSTEM DAMAGE");
	ik_print(screen, font_4x8, 148, y+=8, 0, "Click    to repair");
	ik_dsprite(screen, 172, y, spr_IFarrows->spr[15], 2+(3<<8));
	ik_print(screen, font_4x8, 148, y+=8, 0, "damaged ship systems.");

	y+=6;
	ik_print(screen, font_4x8, 148, y+=8, 0, "Click    to remove");
	ik_dsprite(screen, 172, y, spr_IFarrows->spr[12], 2+(3<<8));
	ik_print(screen, font_4x8, 148, y+=8, 0, "undamaged systems.");
#endif

	ik_blit();

	update_palette();

	end = 0;
	x = key_pressed(key_f[0]); y = 0;
	while (!end && !must_quit)
	{
		ik_eventhandler();
		c = ik_inkey();
		mc = ik_mclick();
		x = key_pressed(key_f[0]); 
		if (!x)
		{
			if (!y)
				y = 1;
			else if (y==2)
				end = 1;
		}
		else if (y)
			y = 2;

		if (mc==1 || c>0)
			end = 1;

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ prep_screen(); ik_blit(); }
	}

	if (must_quit)
		must_quit = 0;
}