// ----------------
//     INCLUDES
// ----------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "typedefs.h"
#include "iface_globals.h"
#include "is_fileio.h"
#include "gfx.h"
#include "snd.h"
#include "interface.h"
#include "cards.h"
#include "combat.h"
#include "textstr.h"

#include "starmap.h"

// ----------------
//     CONSTANTS
// ----------------

#define NUM_KLAITEMS	8
#define NUM_KLAWEP		2
#define NUM_KLASYS		3



char *planet_keywords[plkMax] = 
{
	"PLNT",
	"NAME",
	"TEXT",
	"VALU",
	"END",
};

char *star_keywords[stkMax] = 
{
	"STAR",
	"NAME",
	"TEXT",
	"END",
};

char *item_keywords[itkMax] = 
{
	"ITEM",
	"NAME",
	"TYPE",
	"TEXT",
	"CLAS",
	"COST",
	"FLAG",
	"SOND",
	"END",
};

char *raceflt_keywords[rflMax] = 
{
	"FSET",
	"RACE",
	"SHP1",
	"SHP2",
	"SHP3",
	"FLET",
	"EASY",
	"MEDM",
	"HARD",
	"END",
};

// ----------------
// GLOBAL VARIABLES
// ----------------

t_ik_spritepak	*spr_SMstars;
t_ik_spritepak	*spr_SMstars2;
t_ik_spritepak	*spr_SMplanet;
t_ik_spritepak	*spr_SMplanet2;
t_ik_spritepak	*spr_SMnebula;
t_ik_spritepak	*spr_SMraces;

t_gamesettings	settings;

t_race					races[16];
int32						num_races;

t_planettype		*platypes;
int32						num_platypes;

t_startype			*startypes;
int32						num_startypes;

t_itemtype			*itemtypes;
int32						num_itemtypes;

t_starsystem		*sm_stars;
int32						num_stars;

t_blackhole			*sm_holes;
int32						num_holes;

t_nebula				*sm_nebula;
int32						num_nebula;

t_racefleet			racefleets[16];
int32						num_racefleets;

uint8						*sm_nebulamap;
t_ik_image			*sm_nebulagfx;
t_ik_image			*sm_starfield;

t_fleet					sm_fleets[STARMAP_MAX_FLEETS];

int32						star_env[8][8];
//char						pltype_name[10][32];
int32						plgfx_type[256];
int32						num_plgfx;

char						planetnames[128][32];
int32						planetnametype[128];
int32						num_planetnames;
char						starnames[128][32];
int32						starnametype[128];
int32						num_starnames;
char						holenames[128][32];
int32						num_holenames;

int32						homesystem;

int32						kla_items[32];
int32						kla_numitems;

t_month				months[12] =
{
	{	"Jan", "January",		0,	31 },
	{	"Feb", "February",	31,	28 },
	{	"Mar", "March",			59,	31 },
	{	"Apr", "April",			90,	30 },
	{	"May", "May",				120,	31 },
	{	"Jun", "June",			151,	30 },
	{	"Jul", "July",			181,	31 },
	{	"Aug", "August",		212,	31 },
	{	"Sep", "September",	243,	30 },
	{	"Oct", "October",		273,	31 },
	{	"Nov", "November",	304,	30 },
	{	"Dec", "December",	334,	31 },
};

char	captnames[64][16];
int32 num_captnames;
char	shipnames[64][16];
int32 num_shipnames;

// ----------------
// LOCAL PROTOTYPES
// ----------------

void starmap_initsprites();
void starmap_deinitsprites();
void starmap_deinitterrain();
void starmap_initplanettypes();
void starmap_inititems();
void starmap_deinititems();
void starmap_initracefleets();
void starmap_deinitracefleets();

void starmap_initshipnames();

void starmap_createstars(int n);
void starmap_createholes(int n);
void starmap_createnebula(int n);
void starmap_createnebulamap();
void starmap_createfleets(int32 num);
void starmap_createcards(void);
void starmap_create_klakars(int32 num);

// ----------------
// GLOBAL FUNCTIONS
// ----------------

void starmap_init()
{
	starmap_initsprites();
	starmap_initplanettypes();
	starmap_inititems();
	starmap_initracefleets();
	starmap_initshipnames();
}

void starmap_deinit()
{
	starmap_deinitracefleets();
	starmap_deinititems();
	starmap_deinitsprites();
	starmap_deinitterrain();
}
/*
void waitsecs(int w, int l)
{
	Play_Sound(w, 15, 1);
	start_ik_timer(3, 1000);
	while (!must_quit && get_ik_timer(3)<l)
		ik_eventhandler();
	Stop_Sound(15);
}
*/
void starmap_create()
{
	int y = 0;

#ifdef DEMO_VERSION
	switch (settings.dif_nebula)
	{
		case 0:
		srand( 123456 );
		break;

		case 1:
		srand( 33003773 );
		break;

		case 2:
		srand( 911 );
		break;
	}
#endif

	prep_screen(); ik_print(screen, font_6x8, 8, y+=8, 0, "nebulas..."); ik_blit();
	ik_print_log("creating nebula...\n");
	starmap_createnebula(50+50*settings.dif_nebula);
	prep_screen(); ik_print(screen, font_6x8, 8, y+=8, 0, "stars..."); ik_blit();
	ik_print_log("creating stars...\n");
	starmap_createstars(NUM_STARSYSTEMS);
	prep_screen(); ik_print(screen, font_6x8, 8, y+=8, 0, "blackholes..."); ik_blit();
	ik_print_log("creating black holes...\n");
	starmap_createholes(4);
	prep_screen(); ik_print(screen, font_6x8, 8, y+=8, 0, "nebula gfx..."); ik_blit();
	ik_print_log("creating nebula graphics...\n");
	starmap_createnebulamap();
//	waitsecs(WAV_MUS_DEATH, 1);
#ifdef STARMAP_STEPBYSTEP
	interface_popup(font_6x8, 256,208,128,64,0,0,"pause", "nebula created", "ok");
#endif

#ifdef DEMO_VERSION
	srand( (unsigned)time( NULL ) );
#endif

	prep_screen(); ik_print(screen, font_6x8, 8, y+=8, 0, "discoveries..."); ik_blit();
	ik_print_log("creating discoveries...\n");
	starmap_createcards();
//	waitsecs(WAV_MUS_NEBULA, 1);
#ifdef STARMAP_STEPBYSTEP
	interface_popup(font_6x8, 256,208,128,64,0,0,"pause", "discoveries created", "ok");
#endif
	prep_screen(); ik_print(screen, font_6x8, 8, y+=8, 0, "enemies..."); ik_blit();
	ik_print_log("creating enemies...\n");
	starmap_createfleets(NUM_FLEETS);
//	waitsecs(WAV_MUS_COMBAT, 1);
#ifdef STARMAP_STEPBYSTEP
	interface_popup(font_6x8, 256,208,128,64,0,0,"pause", "enemies created", "ok");
#endif
	prep_screen(); ik_print(screen, font_6x8, 8, y+=8, 0, "klakar..."); ik_blit();
	ik_print_log("creating traders...\n");
	starmap_create_klakars(NUM_KLAITEMS);
//	waitsecs(WAV_KLAKAR, 1);

}

void player_init()
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

	player.system = homesystem;
	player.target = homesystem;
	player.explore = 0;
//	player.card = -1;
	
	player.x = sm_stars[player.system].x;
	player.y = sm_stars[player.system].y;

#ifndef STARMAP_DEBUGFOLD
	player.num_ships = 1;
#else
	player.num_ships = 2;
	player.ships[1] = 29;
#endif

#ifdef STARMAP_DEBUGALLIES
	player.num_ships = 2;
	player.ships[1] = 25;
#endif
	player.ships[0] = 0;

	player.stardate = 12;
	player.hypdate = 0;
	player.foldate = 0;
	player.sel_ship = 0;
	player.sel_ship_time = 0;

	player.num_items = 0; //17;
	for (c = 0; c < player.num_items; c++)
		player.items[c] = rand()%num_itemtypes;

#ifdef STARMAP_DEBUGDEVICES
	for (c = 0; c < num_itemtypes; c++)
	{
		if (itemtypes[c].type == item_device)
			player.items[player.num_items++] = c;
		if (!strcmp(itemtypes[c].name, "Continuum Renderer Array"))
			player.items[player.num_items++] = c;
		if (!strcmp(itemtypes[c].name, "Sardion Optimizer"))
			player.items[player.num_items++] = c;
	}
#endif

#ifdef	STARMAP_DEBUGSYSTEMS
	for (c = 0; c < num_itemtypes; c++)
	{/*
		if (!strcmp(itemtypes[c].name, "Hyperfoam Injectors"))
			player.items[player.num_items++] = c;
		if (!strcmp(itemtypes[c].name, "Multibot Repair Drone"))
			player.items[player.num_items++] = c;
		if (!strcmp(itemtypes[c].name, "Hyperwave Tele-Scrambler"))
			player.items[player.num_items++] = c;
		if (!strcmp(itemtypes[c].name, "Signature Projector"))
			player.items[player.num_items++] = c;*/
		if (!strcmp(itemtypes[c].name, "Hyperwave Filter Array"))
			player.items[player.num_items++] = c;
		if (!strcmp(itemtypes[c].name, "Continuum Renderer Array"))
			player.items[player.num_items++] = c;
	}
#endif

#ifdef STARMAP_DEBUGCLOAK
	for (c = 0; c < num_itemtypes; c++)
	if (itemtypes[c].type == item_system)
	{
		if (shipsystems[itemtypes[c].index].type == sys_misc && shipsystems[itemtypes[c].index].par[0]==1)
			shiptypes[0].system[shiptypes[0].num_systems++] = itemtypes[c].index;
	}
#endif

	for (c = 0; c < num_races; c++)
		races[c].met = 0;
	for (c = 0; c < num_shiptypes; c++)
	{
		shiptypes[c].hits = hulls[shiptypes[c].hull].hits*256;
		for (s = 0; s < shiptypes[c].num_systems; s++)
			shiptypes[c].sysdmg[s] = 0;
	}

	player.bonusdata = 0;

	hud.invslider = 0;
	hud.invselect = -1;
	hud.sysselect = -1;
	hud.sysslider = 0;

	allies_init();
}

void allies_init()
{
	int32 c;
	int32 f;
	int32 s;
	int32 r;
	char name[32];


	for (c = 0; c < num_shiptypes; c++)
	if (shiptypes[c].flag & 1)	// ally
	{
		f = shiptypes[c].flag;
		r = shiptypes[c].race;
		strcpy(name, shiptypes[c].name);
		for (s = 0; s < num_shiptypes; s++)
		if (!(shiptypes[s].flag & 1))
		{
			if (!strcmp(shiptypes[s].name, hulls[shiptypes[c].hull].name))
			{
				memcpy(&shiptypes[c], &shiptypes[s], sizeof(t_shiptype));
				strcpy(shiptypes[c].name, name);
				shiptypes[c].flag = f;
				shiptypes[c].race = r;
			}
		}
	}
}

// ----------------
// LOCAL FUNCTIONS
// ----------------

void starmap_initsprites()
{
	t_ik_image *pcx;	
	int n;

	spr_SMstars		= load_sprites("graphics/smstars.spr");
	spr_SMstars2	= load_sprites("graphics/smstars2.spr");
	spr_SMplanet	= load_sprites("graphics/smplanet.spr");
	spr_SMplanet2 = load_sprites("graphics/smplnet2.spr");
	spr_SMnebula	= load_sprites("graphics/smnebula.spr");
	spr_SMraces		= load_sprites("graphics/smraces.spr");
	
	pcx = NULL;

	if (!spr_SMstars)
	{
		pcx = ik_load_pcx("starmap.pcx", NULL);
		spr_SMstars = new_spritepak(9);
		for (n=0;n<8;n++)
		{
			spr_SMstars->spr[n] = get_sprite(pcx, n*32, 0, 32, 32);
		}
		spr_SMstars->spr[8] = get_sprite(pcx, 192, 64, 128, 128);
		save_sprites("graphics/smstars.spr", spr_SMstars);
	}

	if (!spr_SMplanet)
	{
		if (!pcx)
			pcx = ik_load_pcx("starmap.pcx", NULL);
		spr_SMplanet = new_spritepak(11);
		for (n=0;n<11;n++)
		{
			spr_SMplanet->spr[n] = get_sprite(pcx, n*16, 64, 16, 16);
		}

		save_sprites("graphics/smplanet.spr", spr_SMplanet);
	}

	if (!spr_SMnebula)
	{
		if (!pcx)
			pcx = ik_load_pcx("starmap.pcx", NULL);
		spr_SMnebula = new_spritepak(9);
		for (n=0;n<8;n++)
		{
			spr_SMnebula->spr[n] = get_sprite(pcx, n*32, 32, 32, 32);
		}
		spr_SMnebula->spr[8] = get_sprite(pcx, 0, 80, 128, 128);

		save_sprites("graphics/smnebula.spr", spr_SMnebula);
	}

	if (pcx)
		del_image(pcx);

	if (!spr_SMplanet2)
	{
		pcx = ik_load_pcx("planets.pcx", NULL);
		spr_SMplanet2 = new_spritepak(23);
		for (n=0;n<23;n++)
		{
			spr_SMplanet2->spr[n] = get_sprite(pcx, (n%5)*64, (n/5)*64, 64, 64);
		}

		save_sprites("graphics/smplnet2.spr", spr_SMplanet2);
		del_image(pcx);
	}
	if (!spr_SMstars2)
	{
		pcx = ik_load_pcx("suns.pcx", NULL);
		spr_SMstars2 = new_spritepak(8);
		for (n=0;n<8;n++)
		{
			spr_SMstars2->spr[n] = get_sprite(pcx, (n%4)*64, (n/4)*64, 64, 64);
		}

		save_sprites("graphics/smstars2.spr", spr_SMstars2);
		del_image(pcx);
	}
	if (!spr_SMraces)
	{
#ifndef DEMO_VERSION
		pcx = ik_load_pcx("races.pcx", NULL);
		spr_SMraces = new_spritepak(15);
		for (n=0;n<15;n++)
#else
		pcx = ik_load_pcx("../demogfx/races.pcx", NULL);
		spr_SMraces = new_spritepak(8);
		for (n=0;n<8;n++)
#endif
		{
			spr_SMraces->spr[n] = get_sprite(pcx, (n%5)*64, (n/5)*64, 64, 64);
		}

		save_sprites("graphics/smraces.spr", spr_SMraces);
		del_image(pcx);
	}
}

void starmap_deinitsprites()
{
	free_spritepak(spr_SMstars);
	free_spritepak(spr_SMstars2);
	free_spritepak(spr_SMplanet);
	free_spritepak(spr_SMplanet2);
	free_spritepak(spr_SMnebula);
	free_spritepak(spr_SMraces);
}

void starmap_initplanettypes()
{
	FILE* ini;
	char s1[64], s2[256];
	char end;
	int num;
	int flag;
	int n;
	int com;
	int tv1;

	ini = myopen("gamedata/planets.ini", "rb");
	if (!ini)
		return;

	end = 0; num = 0; n = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		if (!strcmp(s1, planet_keywords[plkBegin]))
			num++;
		if (!strcmp(s1, star_keywords[stkBegin]))
			n++;
	}
	fclose(ini);

	platypes = (t_planettype*)calloc(num, sizeof(t_planettype));
	if (!platypes)
		return;
	num_platypes = num;

	startypes = (t_startype*)calloc(n, sizeof(t_startype));
	if (!startypes)
		return;
	num_startypes = n;

	ini = myopen("gamedata/planets.ini", "rb");
	end = 0; num = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		com = -1;
		for (n = 0; n < plkMax; n++)
			if (!strcmp(s1, planet_keywords[n]))
				com = n;

		if (flag == 0)
		{
			if (com == plkBegin)
			{
				flag = 1;
			}
		}
		else switch(com)
		{
			case plkName:
			strcpy(platypes[num].name, s2);
			break;

			case plkText:
			strcpy(platypes[num].text, s2);
			break;

			case plkBonus:
			sscanf(s2, "%d", &tv1);
			platypes[num].bonus = tv1;
			break;

			case plkEnd:
			num++; flag = 0;
			break;

			default: ;
		}

	}
	fclose(ini);

	ini = myopen("gamedata/planets.ini", "rb");
	end = 0; num = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		com = -1;
		for (n = 0; n < stkMax; n++)
			if (!strcmp(s1, star_keywords[n]))
				com = n;

		if (flag == 0)
		{
			if (com == stkBegin)
			{
				flag = 1;
			}
		}
		else switch(com)
		{
			case stkName:
			strcpy(startypes[num].name, s2);
			break;

			case stkText:
			strcpy(startypes[num].text, s2);
			break;

			case stkEnd:
			num++; flag = 0;
			break;

			default: ;
		}

	}
	fclose(ini);

	ini = myopen("gamedata/planets.ini", "rb");
	if (!ini)
		return;

	num_planetnames = 0; num_starnames = 0; num_holenames = 0; num_plgfx = 0;
	end = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		if (!strcmp(s1, "END"))
			flag = 0;
		else if (!strcmp(s1, "ENVIROS"))
		{	num=0; flag=2; }
		else if (!strcmp(s1, "PLANETGFX"))
		{	num=0; flag=3; }
		else if (!strcmp(s1, "PLANETNAMES"))
		{	num=0; flag=4; }
		else if (!strcmp(s1, "STARNAMES"))
		{	num=0; flag=5; }
		else if (!strcmp(s1, "HOLENAMES"))
		{	num=0; flag=6; }
		else
		{
			if (flag==2) // enviro
			{
				for (n=0; n<8; n++)
					star_env[num][n]=s1[n]-'0';
				num++;
			}
			if (flag==3) // planetgfx
			{
				for (n=0; n<num_platypes; n++)
					if (!strcmp(s1, platypes[n].name))
						plgfx_type[num_plgfx++]=n;
				num++;
			}
			if (flag==4) // planetnames
			{
				strcpy(planetnames[num], s2);
				for (n=0; n<num_platypes; n++)
					if (!strcmp(s1, platypes[n].name))
						planetnametype[num] = n;
				num++;
				num_planetnames = num;
			}
			if (flag==5) // starnames
			{
				strcpy(starnames[num], s2);
				for (n=0; n<8; n++)
					if (!strcmp(s1, startypes[n].name))
						starnametype[num] = n;
				num++;
				num_starnames = num;
			}
			if (flag==6) // holenames
			{
				strcpy(holenames[num], s2);
				num++;
				num_holenames = num;
			}
		}

	}
	fclose(ini);
}

void starmap_inititems()
{
	FILE* ini;
	char s1[64], s2[256];
	char end;
	int num;
	int flag;
	int n, com;
	int tv1;

	char itemtype[8][32];
	int32 num_types;

	ini = myopen("gamedata/items.ini", "rb");
	if (!ini)
		return;

	end = 0; num = 0; 
	flag = 0; num_types = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		if (!strcmp(s1, item_keywords[itkBegin]))
			num++;
		if (!strcmp(s1, "ITEMTYPES"))
		{	flag = 1; n=0; }
		else if (flag>0 && strcmp(s1, "END")==0)
			flag = 0;
		else 	if (flag == 1)
		{
			strcpy(itemtype[num_types], s1);
			num_types++;
		}

	}
	fclose(ini);

	itemtypes = (t_itemtype*)calloc(num, sizeof(t_itemtype));
	if (!itemtypes)
		return;
	num_itemtypes = num;

	ini = myopen("gamedata/items.ini", "rb");

	end = 0; num = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		com = -1;
		for (n = 0; n < itkMax; n++)
			if (!strcmp(s1, item_keywords[n]))
				com = n;

		if (flag == 0)
		{
			if (com == itkBegin)
			{
				itemtypes[num].flag = 0;
				itemtypes[num].index = -1;
				flag = 1;
			}
		}
		else switch(com)
		{
			case itkName:
			strcpy(itemtypes[num].name, s2);
			break;

			case itkType:
			for (n = 0; n < num_types; n++)
				if (!strcmp(s2, itemtype[n]))
					itemtypes[num].type = n;
			/*if (itemtypes[num].type == item_weapon)
			{
				for (n = 0; n < num_shipweapons; n++)
					if (!strcmp(shipweapons[n].name, itemtypes[num].name))
					{	itemtypes[num].index = n; shipweapons[n].item = num; }
			}*/
			if (itemtypes[num].type == item_weapon || itemtypes[num].type == item_system)
			{
				for (n = 0; n < num_shipsystems; n++)
					if (!strcmp(shipsystems[n].name, itemtypes[num].name))
					{	
						itemtypes[num].index = n; shipsystems[n].item = num; 
						if (shipsystems[n].type == sys_weapon)
							shipweapons[shipsystems[n].par[0]].item = num;
					}
			}
			break;

			case itkText:
			strcpy(itemtypes[num].text, s2);
			break;

			case itkClass:
			strcpy(itemtypes[num].clas, s2);
			break;

			case itkCost:
			sscanf(s2, "%d", &tv1);
			itemtypes[num].cost = tv1;
			break;

			case itkFlag:
			sscanf(s2, "%d", &tv1);
			itemtypes[num].flag = tv1;
			break;

			case itkSound:
			sscanf(s2, "%d", &tv1);
			if (!strcmp(itemtypes[num].clas, textstring[STR_INV_ARTIFACT]))
				itemtypes[num].sound = tv1 + SND_ARTIF*( (tv1>=0)-(tv1<0) );
			else
				itemtypes[num].sound = tv1 + SND_ITEMS*( (tv1>=0)-(tv1<0) );
			break;

			case itkEnd:
			num++; flag = 0;
			break;

			default: ;
		}

	}
	fclose(ini);
}

void starmap_deinititems()
{
	num_itemtypes = 0;
	free(itemtypes);
}

void starmap_deinitterrain()
{
	if (sm_stars)   free(sm_stars);
	num_stars = 0;
	if (sm_holes)   free(sm_holes);
	num_holes = 0;
	if (sm_nebula)  free(sm_nebula);
	num_nebula = 0;

	if (num_platypes)	free(platypes);
	num_platypes = 0;

	if (num_startypes)	free(startypes);
	num_startypes = 0;

	if (sm_nebulamap) free(sm_nebulamap);
	del_image(sm_nebulagfx);
	del_image(sm_starfield);
}

void starmap_createstars(int n)
{
	int32 c;
	int32 end;
	int32 r;
	int32 t;
	int32 h;
//	int32 x,y,x1,y1;

	num_stars = n;
#ifndef DEMO_VERSION
	sm_stars = (t_starsystem *)calloc(n + 1, sizeof(t_starsystem));
#else
	sm_stars = (t_starsystem *)calloc(n, sizeof(t_starsystem));
#endif

	// generate star locations
	for (c = 0; c < num_stars; c++)
	{
		end = 0;
		while (!end && !must_quit)
		{
			end = 1;
			sm_stars[c].x = rand()%420 - 210;
#ifndef DEMO_VERSION
			sm_stars[c].y = rand()%364 - 182;
#else
			sm_stars[c].y = -176 + 352 * c / (num_stars-1);
#endif
			sm_stars[c].color = rand()%8;
			for (t = 0; t < c; t++)
			{
				r = (int32)sqrt( (sm_stars[t].x-sm_stars[c].x)*(sm_stars[t].x-sm_stars[c].x) +
												 (sm_stars[t].y-sm_stars[c].y)*(sm_stars[t].y-sm_stars[c].y) );
				if (r < 64)
					end = 0;
			}
		}
		// create planet
		sm_stars[c].planet = star_env[sm_stars[c].color][rand()%8];
		sm_stars[c].novadate = 0;
		sm_stars[c].novatime = 0;
		end=0;
		while (!end && !must_quit)
		{
			ik_eventhandler();
			end=1;
			sm_stars[c].planetgfx = rand()%num_plgfx;
			if (plgfx_type[sm_stars[c].planetgfx]!=sm_stars[c].planet)
				end=0;
			r=1;
			for (t = 0; t < c; t++)
			{
				if (sm_stars[c].planet == sm_stars[t].planet)
					r++;
			}
			h=0;
			for (t = 0; t < num_plgfx; t++)
			{
				if (plgfx_type[t] == sm_stars[c].planet)
					h++;
			}
			if (h >= r)
			{
				for (t = 0; t < c; t++)
				{
					if (sm_stars[c].planetgfx == sm_stars[t].planetgfx)
						end=0;
				}
			}
		}
#ifndef STARMAP_DEBUGINFO
		sm_stars[c].explored = 0;
#else
		sm_stars[c].explored = 1;
#endif
		// pick names
		end = 0;
		while (!end && !must_quit)
		{
			ik_eventhandler();
			end = 0;
			while (!end)
			{
				r = rand()%num_starnames;
				if (starnametype[r] == sm_stars[c].color)
					end=1;
			}
			for (t = 0; t < c; t++)
				if (!strcmp(sm_stars[t].starname, starnames[r]))
					end = 0;
		}
		strcpy(sm_stars[c].starname, starnames[r]);
		end = 0;
		while (!end && !must_quit)
		{
			ik_eventhandler();
			end = 0;
			while (!end)
			{
				r = rand()%num_planetnames;
				if (planetnametype[r] == sm_stars[c].planet)
					end=1;
			}
			for (t = 0; t < c; t++)
				if (!strcmp(sm_stars[t].planetname, planetnames[r]))
					end = 0;
		}
		strcpy(sm_stars[c].planetname, planetnames[r]);
	}

	// find suitable home (starting) world
	h = -1;
	t = 640;
	for (c = 0; c < num_stars; c++)
	{
		if (!sm_nebulamap[((240-sm_stars[c].y)<<9)+(240+sm_stars[c].x)])
		{
			r = (int32)sqrt( (sm_stars[c].x-0)*(sm_stars[c].x-0) +
											 (sm_stars[c].y-(-210))*(sm_stars[c].y-(-210)) );
			if (r < t)
			{
				t = r;
				h = c;
			}
		}
	}
	homesystem = h;
	if (h>-1)
	{
		sm_stars[h].color = 3;
		sm_stars[h].planet = 9;
		sm_stars[h].planetgfx = 21;
		sm_stars[h].explored = 2;
		strcpy(sm_stars[h].starname, textstring[STR_NAME_GLORY]);
		strcpy(sm_stars[h].planetname, textstring[STR_NAME_HOPE]);
	}

	// extra star for kawangi start
#ifndef DEMO_VERSION
	sm_stars[num_stars].x = 0;
	sm_stars[num_stars].y = 255;
	sm_stars[num_stars].color = 0;
	sm_stars[num_stars].novadate = 0;
	sm_stars[num_stars].ds_x = SM_MAP_X + 240 + sm_stars[num_stars].x;
	sm_stars[num_stars].ds_y = SM_MAP_Y + 240 - sm_stars[num_stars].y;
#endif

	/*
	// draw nebular glow
	for (c = 0; c < num_stars; c++)
	{
		x = 240+sm_stars[c].x;
		y = 240-sm_stars[c].y;
		for (y1 = y-64; y1 < y+64; y1++)
		if (y1>=0 && y1<480)
			for (x1 = x-64; x1 < x+64; x1++)
			if (x1>=0 && x1<480)
			{
				if (sm_nebulamap[(y1<<9)+x1])
				{
					r = (int32)sqrt( (x1-x)*(x1-x) + (y1-y)*(y1-y) );
					if (r < 64)
					{
						t = (63-r)/8 + sm_stars[c].color*16;
						t = gfx_addbuffer[(t<<8) + ik_getpixel(sm_nebulagfx, x1, y1)];
						ik_putpixel(sm_nebulagfx, x1, y1, t);
					}
				}
			}
	}
	*/

	/*
	for (c = 0; c < num_stars; c++)
	{
		sm_stars[c].x <<= 8;
		sm_stars[c].y <<= 8;
	}*/

}

void starmap_createnebula(int n)
{
	int32 c, t, r;
	int32 end;
	int32 tries;

	int32 num_groups;

	num_groups = rand()%3 + 2;

	num_nebula = n;
	sm_nebula = (t_nebula *)calloc(num_nebula, sizeof(t_nebula));

	sm_nebulamap = (uint8 *)calloc(512*480, 1);
	sm_nebulagfx = ik_load_pcx("graphics/backgrnd.pcx", NULL); //new_image(480,480);
	sm_starfield = ik_load_pcx("graphics/backgrnd.pcx", NULL); //new_image(480,480);

	for (c = 0; c < num_groups; c++)
	{
		sm_nebula[c].x = rand()%360 + 60;
		sm_nebula[c].y = rand()%240 + 100;
		sm_nebula[c].sprite = rand()%7;
	}

	for (c = num_groups; c < num_nebula; c++)
	{
		end = 0;
		tries = 0;
		while (end != 1)
		{
			end = -1;
			tries++;
			sm_nebula[c].x = rand()%480;
			sm_nebula[c].y = rand()%360;
			sm_nebula[c].sprite = rand()%7;
			for (t = 0; t < c; t++)
			{
				r = (int32)sqrt(	(sm_nebula[t].x - sm_nebula[c].x)*(sm_nebula[t].x - sm_nebula[c].x)	+
													(sm_nebula[t].y - sm_nebula[c].y)*(sm_nebula[t].y - sm_nebula[c].y)	);
				if (r < 24)
				{
					if ((t%num_groups == c%num_groups) || (tries>=100))
					{	
						if (end == -1)
							end = 1;
						if (r < 12 && tries<100)
							end = 0;
					}
					if (t%num_groups != c%num_groups)
						end = 0;
				}
			}
		}
	}

}

void starmap_createnebulamap()
{
	int32 c, t, r;
	int32 x, y, x1, y1;
	uint8 *data;

	for (c = 0; c < num_nebula; c++)
	{
		x = sm_nebula[c].x; y = sm_nebula[c].y;
		for (y1 = sm_nebula[c].y - 32; y1 < sm_nebula[c].y + 31; y1++)
		if (y1>=0 && y1 < 480)
			for (x1 = sm_nebula[c].x - 32; x1 < sm_nebula[c].x + 31; x1++)
			if (x1>=0 && x1 < 480)
			{
				x = (x1+32-sm_nebula[c].x); y = (y1+32-sm_nebula[c].y);
				data = spr_SMnebula->spr[sm_nebula[c].sprite]->data;
				r = (data[(y>>1)*32+(x>>1)] +
						data[(y>>1)*32+(x>>1)+1]*(x&1) +
						data[(y>>1)*32+(x>>1)+32]*(y&1) +
						data[(y>>1)*32+(x>>1)+33]*(x&1)*(y&1)) * 4 / (1+(x&1)+(y&1)+(x&1)*(y&1));
				t = sm_nebulamap[(y1<<9)+x1];
				sm_nebulamap[(y1<<9)+x1] = MIN(t + r, 255);
			}
	}

	for (y = 0; y < 480; y++)
		for (x = 0; x < 480; x++)
		{
			t = sm_nebulamap[(y<<9)+x];
			if (t < 64)
				sm_nebulamap[(y<<9)+x] = 0;
			else
				sm_nebulamap[(y<<9)+x] = t - 63;
		}

	for (c = 0; c < num_holes; c++)
	{
		t = sm_nebulamap[((240-sm_holes[c].y)<<9)+(240+sm_holes[c].x)];
		if (t > 0)
			sm_holes[c].explored = 1;
		for (y1 = 240-sm_holes[c].y - 32; y1 < 240-sm_holes[c].y + 31; y1++)
		if (y1>=0 && y1 < 480)
			for (x1 = 240+sm_holes[c].x - 32; x1 < 240+sm_holes[c].x + 31; x1++)
			if (x1>=0 && x1 < 480)
			{
				x = (x1+32-(240+sm_holes[c].x)); y = (y1+32-(240-sm_holes[c].y));
				data = spr_SMnebula->spr[7]->data;
				r = (data[(y>>1)*32+(x>>1)] +
						data[(y>>1)*32+(x>>1)+1]*(x&1) +
						data[(y>>1)*32+(x>>1)+32]*(y&1) +
						data[(y>>1)*32+(x>>1)+33]*(x&1)*(y&1)) * 4 / (1+(x&1)+(y&1)+(x&1)*(y&1));
				t = sm_nebulamap[(y1<<9)+x1];
				if (r < 60)
					sm_nebulamap[(y1<<9)+x1] = (MAX(0,t-60+r) * r) / (15 * 4);
			}
	}

	starmap_createnebulagfx();
}

void starmap_createnebulagfx()
{
	int32 c, t;
	int32 x, y;

	ik_copybox(sm_starfield, sm_nebulagfx, 0, 0, 480, 480, 0, 0);

	for (y = 0; y < 480; y++)
		for (x = 0; x < 480; x++)
		{
			t = sm_nebulamap[(y<<9)+x];
			if (!t)
			{	
				c = 0; 
			}
			else
			{	
				if (t < 14)
					c = 9*16+t/2+2;
				else if (t < 70)
					c = 9*16+8-(t-14)/8;
				else
					c = 9*16+2;
				/*
				if (t < 14)
					c = 9*16+t/2+2;
				else if (t < 70)
					c = 9*16+8-(t-14)/8;
				else
					c = 9*16+2;
				*/
			}
			if (c)
				ik_putpixel(sm_nebulagfx, x, y, c);
		}

	// grid lines
	for (y = 0; y < 480; y++)
		for (x = 16; x < 480; x+=64)
		{
			ik_putpixel(sm_nebulagfx, x, y, gfx_addbuffer[(2<<8)+ik_getpixel(sm_nebulagfx, x, y)] );
		}
	for (x = 0; x < 480; x++)
		for (y = 16; y < 480; y+=64)
		{
			ik_putpixel(sm_nebulagfx, x, y, gfx_addbuffer[(2<<8)+ik_getpixel(sm_nebulagfx, x, y)] );
		}
}

void starmap_createholes(int32 n)
{
	int32 c;
	int32 end;
	int32 r;
	int32 t;

	num_holes = n;
	sm_holes = (t_blackhole *)calloc(num_holes, sizeof(t_blackhole));

	for (c = 0; c < num_holes; c++)
	{
		end = 0;
		while (!end)
		{
			end = 1;

			sm_holes[c].x = rand()%420 - 210;
			sm_holes[c].y = rand()%400 - 200;

			for (t = 0; t < num_stars; t++)
			{
				r = get_distance ( sm_stars[t].x - sm_holes[c].x,
													 sm_stars[t].y - sm_holes[c].y );
				if (r < 40 || (t == homesystem && r < 96))
					end = 0;
			}
			/*
			r = get_distance ( sm_stars[homesystem].x - sm_holes[c].x,
												 sm_stars[homesystem].y - sm_holes[c].y );
			if (r < 64)
				end = 0;
			*/
			for (t = 0; t < c; t++)
			{
				r = get_distance ( sm_holes[t].x - sm_holes[c].x,
													 sm_holes[t].y - sm_holes[c].y );
				if (r < 96)
					end = 0;
			}
		}
		sm_holes[c].explored = 0;
		sm_holes[c].size = 32;
		end = 0; // name
		while (!end && !must_quit)
		{
			ik_eventhandler();
			end = 1;
			r = rand()%num_holenames;
			for (t = 0; t < c; t++)
				if (!strcmp(sm_holes[t].name, holenames[r]))
					end = 0;
		}
		strcpy(sm_holes[c].name, holenames[r]);
	}
}

void starmap_createfleets(int32 num) // create enemy fleets
{
	int32 c;
	int32 s;
	int32 n;
	int32 kla;
	int32 end;
	int32 tries;
#ifndef DEMO_VERSION
	int32 muk = 0, mukr = 0;
	int32 kaw = 0;
#endif

	int32 dif=settings.dif_enemies;	

#ifndef DEMO_VERSION
	for (c = 0; c < num_stars; c++)
	{
		if (ecards[sm_stars[c].card].type == card_ally)
			if (shiptypes[ecards[sm_stars[c].card].parm].race == race_muktian)
				muk = shiptypes[ecards[sm_stars[c].card].parm].race;
		if (ecards[sm_stars[c].card].type == card_rareitem)
			if (itemtypes[ecards[sm_stars[c].card].parm].flag & device_collapser)
				kaw = race_kawangi;
//			!strcmp(ecards[sm_stars[c].card].name, "Limited Vacuum Collapser"))
	}

#ifdef STARMAP_KAWANGI
	kaw = race_kawangi;
#endif

	if (kaw)
		num++;
#endif

#ifndef DEMO_VERSION
	mukr = race_muktian;
#endif

	kla = race_klakar;

	for (c = 0; c < STARMAP_MAX_FLEETS; c++)
	{
		sm_fleets[c].num_ships = 0;
		sm_fleets[c].explored = 0;
		sm_fleets[c].enroute = 0;
		sm_fleets[c].blowtime = 0;
	}

	for (c = 0; c < num; c++)
	{
		end = 0;
#ifndef STARMAP_DEBUGENEMIES
		sm_fleets[c].explored = 0;
#else
		sm_fleets[c].explored = 2;
#endif
		tries = 0;

		while (!end && !must_quit)
		{
			ik_eventhandler();
			end=1;
			tries++;

//			if (tries < 100)
//				prep_screen(); ik_print(screen, font_6x8, tries*6, 400+c*8, 0, "."); ik_blit();

			// race
			if (kla)
			{
				sm_fleets[c].race = kla;
			}
#ifndef DEMO_VERSION
			else if (muk)
			{
				sm_fleets[c].race = muk;
			}
			else if (kaw)
			{
				sm_fleets[c].race = kaw;
				Play_SoundFX(WAV_DOT);
			}
#endif
			else
#ifndef STARMAP_DEBUGTANRU
				sm_fleets[c].race = enemies[rand()%num_enemies];
#else
				sm_fleets[c].race = race_tanru;
#endif

			// ships
			if (sm_fleets[c].race == kla || races[sm_fleets[c].race].fleet==-1)
			{
				sm_fleets[c].num_ships = 1;
				sm_fleets[c].ships[0] = 0;
				while ((shiptypes[sm_fleets[c].ships[0]].race != sm_fleets[c].race || 
								shiptypes[sm_fleets[c].ships[0]].flag == 1) &&
								!must_quit)
				{
					ik_eventhandler();
					sm_fleets[c].ships[0] = rand()%num_shiptypes;
				}
			}
			else
			{
				end = 0;
				while (!end)
				{
					sm_fleets[c].num_ships = 0;
					n = racefleets[races[sm_fleets[c].race].fleet].diff[dif][rand()%10];
					for (s=2; s>=0; s--)
					{
						end = racefleets[races[sm_fleets[c].race].fleet].fleets[n][s];
						while (end>0) // && sm_fleets[c].num_ships<8)
						{
							sm_fleets[c].ships[sm_fleets[c].num_ships] = racefleets[races[sm_fleets[c].race].fleet].stype[s];
							sm_fleets[c].num_ships++;
							end--;
						}
					}
					end = 1;
					// check for identical fleets and prevent if possible
					if (tries < 100)
					{
						for (s = 0; s < c; s++)
						if (sm_fleets[s].race == sm_fleets[c].race && sm_fleets[s].num_ships == sm_fleets[c].num_ships)
						{
							end = 0;
							for (n = 0; n < sm_fleets[c].num_ships; n++)
								if (sm_fleets[s].ships[n] != sm_fleets[c].ships[n])
									end = 1;
						}
					}
				}
				
			}

			// location
#ifndef DEMO_VERSION
			if (sm_fleets[c].race == race_kawangi)
			{
				sm_fleets[c].system = num_stars;
				sm_fleets[c].explored = 0;
				starmap_kawangimove(c);
			}
			else
			{
#endif
				sm_fleets[c].system = rand()%num_stars;
				sm_fleets[c].target = sm_fleets[c].system;
				if (sm_fleets[c].system==homesystem)
					end = 0;
				if (tries<100)
				{
					if (sm_fleets[c].race == kla)
					{
						if (get_distance( sm_stars[sm_fleets[c].system].x - sm_stars[homesystem].x, sm_stars[sm_fleets[c].system].y - sm_stars[homesystem].y ) > 150)
							end = 0;
					}
					else
						if (get_distance( sm_stars[sm_fleets[c].system].x - sm_stars[homesystem].x, sm_stars[sm_fleets[c].system].y - sm_stars[homesystem].y ) < 150)
							end = 0;
#ifndef DEMO_VERSION
					if (!strcmp(ecards[sm_stars[sm_fleets[c].system].card].name, textstring[STR_EVENT_NOVA]))
						end = 0;
#endif
				}
				n = 0;
				for (s=0; s<c; s++)
				{
					if (sm_fleets[s].system==sm_fleets[c].system)
						end = 0;
					if (sm_fleets[s].race==sm_fleets[c].race)
						n++;
				}
#ifdef STARMAP_DEBUGTANRU
				if (n > 1) n = 1;
#endif
#ifndef DEMO_VERSION
				if (n > 1 || (n==1 && sm_fleets[c].race == race_muktian))
					end = 0;
#endif
				if (ecards[sm_stars[sm_fleets[s].system].card].type == card_ally)
					end = 0;
#ifndef DEMO_VERSION
				else if (!strcmp(ecards[sm_stars[sm_fleets[s].system].card].name, textstring[STR_EVENT_HULK]))
					end = 0;
			}
#endif
		}
/*
		prep_screen(); 
//		ik_drawbox(screen, 0, 360+c*8, 128, 368+c*8, 0);
		ik_print(screen, font_6x8, 0, 360+c*16, 0, "%s %d", races[sm_fleets[c].race].name, sm_fleets[c].num_ships); 
		for (n = 0; n < sm_fleets[c].num_ships; n++)
		{
			ik_drsprite(screen, 128+n*16, 364+c*16, 0, 16, hulls[shiptypes[sm_fleets[c].ships[n]].hull].sprite, 0);
		}
		ik_blit();
*/
#ifndef DEMO_VERSION
		if (sm_fleets[c].race == race_muktian && muk > 0)
			muk = 0;
		if (sm_fleets[c].race == race_kawangi && kaw > 0)
			kaw = 0;
#endif
		if (sm_fleets[c].race == race_klakar && kla > 0)
			kla = 0;
	}

#ifndef DEMO_VERSION
	for (n = 0; n < num_stars; n++)
	{
		if (!strcmp(ecards[sm_stars[n].card].name, textstring[STR_EVENT_HULK]))
		{
			c = num;
			sm_fleets[c].explored = 0;
			sm_fleets[c].num_ships = 1;
			sm_fleets[c].race = race_unknown;
			sm_fleets[c].system = n;
			sm_fleets[c].enroute = 0;
			sm_fleets[c].target = n;
			end = 0;
			while (!end && !must_quit)
			{
				ik_eventhandler();
				end = 1;
				sm_fleets[c].ships[0] = rand()%num_shiptypes;
				if (shiptypes[sm_fleets[c].ships[0]].race != race_unknown)
					end = 0;
			}
		}
	}
#endif

}

void starmap_createcards(void)
{
	int32 c, s;
	int32 i;
	int32 t;
	int32 tries;
	int32 end;
#ifdef STARMAP_STEPBYSTEP
	char texty[256];
#endif

	int32 nit = 0, nri = 0, nev = 0;

	for (c = 0; c < num_stars; c++)
		sm_stars[c].card = -1;

	for (c = 0; c < NUM_LIFEFORMS; c++)
	{
		end = 0;
		while (!end)
		{
			end = 1;
			s = rand()%num_stars;
			if (s == homesystem)
				end = 0;
			if (sm_stars[s].planet == 0 || sm_stars[s].planet > 5)
				end = 0;
			if (sm_stars[s].card > -1)
				end = 0;
		}
		end = 0;
		while (!end)
		{
			end = 1;
			i = rand()%num_ecards;
			if (ecards[i].type != card_lifeform)
				end = 0;
			for (t = 0; t < num_stars; t++)
			if (t != homesystem && t != s)
				if (sm_stars[t].card == i)
					end = 0;
		}
		sm_stars[s].card = i;
	}

#ifdef STARMAP_STEPBYSTEP
	interface_popup(font_6x8, 256,208,128,64,0,0,"pause", "lifeforms created", "ok");
#endif

	for (c = 0; c < NUM_ALLIES; c++)
	{
		end = 0;
		while (!end)
		{
			end = 1;
			i = rand()%num_ecards;
			if (ecards[i].type != card_ally)
				end = 0;
			else
			{
				for (t = 0; t < num_stars; t++)
				if (t != homesystem && sm_stars[t].card > -1)
				{
					if (sm_stars[t].card == i)
						end = 0;
					if (ecards[sm_stars[t].card].type == card_ally)
						if (shiptypes[ecards[sm_stars[t].card].parm].race == shiptypes[ecards[i].parm].race)
							end = 0;
				}
			}
		}

#ifdef STARMAP_STEPBYSTEP
	sprintf(texty, "ally %d 50%%", c);
	interface_popup(font_6x8, 256,208,128,64,0,0,"pause", texty, "ok");
#endif

		end = 0; tries = 0;
		while (!end)
		{
			end = 1; tries++;
			s = rand()%num_stars;
			if (s == homesystem)
				end = 0;
			if (sm_stars[s].card > -1)
				end = 0;
			if (tries < 100)
			{
				if (sm_stars[s].planet == 0 || sm_stars[s].planet > 6)
					end = 0;
				if (shiptypes[ecards[i].parm].race == 1)
				{
					if (get_distance( sm_stars[s].x-sm_stars[homesystem].x, sm_stars[s].y-sm_stars[homesystem].y ) > 200)
						end = 0;
				}
			}
		}
		sm_stars[s].card = i;

#ifdef STARMAP_STEPBYSTEP
	sprintf(texty, "ally %d 100%%", c);
	interface_popup(font_6x8, 256,208,128,64,0,0,"pause", texty, "ok");
#endif


	}

#ifdef STARMAP_STEPBYSTEP
	interface_popup(font_6x8, 256,208,128,64,0,0,"pause", "allies created", "ok");
#endif

	for (c = 0; c < num_stars; c++)
	if (c != homesystem && sm_stars[c].card == -1)
	{
		end = 0;
		while (!end)
		{
			end = 1;
			sm_stars[c].card = 1+rand()%(num_ecards-1);
			t = ecards[sm_stars[c].card].type;
			if (t == card_ally || t == card_lifeform)
				end = 0;

			if (t == card_item && nit >= NUM_ITEMS)					end = 0;
			if (t == card_rareitem && nri >= NUM_RAREITEMS) end = 0;
			if (t == card_event && nev >= NUM_EVENTS)				end = 0;

			for (s = 0; s < c ; s++)
			if (s != homesystem)
				if (sm_stars[s].card == sm_stars[c].card)
					end = 0;

			if (end)
			{
				if (t == card_item)						nit++;
				else if (t == card_rareitem)  nri++;
				else if (t == card_event)			nev++;

#ifndef DEMO_VERSION
				if (!strcmp(ecards[sm_stars[c].card].name, textstring[STR_EVENT_GIANT]))
				{
					sm_stars[c].planet = 8;
					end = 0;
					while (!end)
					{
						end = rand()%num_planetnames;
						if (planetnametype[end] == sm_stars[c].planet)
						{	strcpy(sm_stars[c].planetname, planetnames[end]); end = 1; }
						else
							end=0;
					}
					end = 0;
					while (!end)
					{
						end = 1;
						sm_stars[c].planetgfx = rand()%num_plgfx;
						if (plgfx_type[sm_stars[c].planetgfx]!=sm_stars[c].planet)
							end=0;
					}
				}
#endif
				end = 1;
			}
		}
	}
}

void starmap_create_klakars(int32 num)
{
	int32 c;
	int32 s;
	int32 t;
	int32 n;
	int32 end;

	kla_numitems = num;

	c = 0;
	for (n = 0; n < NUM_KLAWEP; n++)
	{
		end = 0;
		while (!end)
		{
			end = 1;
			kla_items[c] = rand()%num_itemtypes;
			if (itemtypes[kla_items[c]].flag & 1)
				end = 0;
			if (itemtypes[kla_items[c]].type != item_weapon)
				end = 0;
			for (t = 0; t < c; t++)
				if (kla_items[t] == kla_items[c])
					end = 0;
			for (s = 0; s < num_stars; s++)
			if (s != homesystem && sm_stars[s].card > -1)
			{
				t = ecards[sm_stars[s].card].type;
				if (t == card_item || t == card_rareitem || t == card_lifeform)
				{
					if (ecards[sm_stars[s].card].parm == kla_items[c])
						end = 0;
				}
			}

		}
		c++;
	}

	for (n = 0; n < NUM_KLASYS; n++)
	{
		end = 0;
		while (!end)
		{
			end = 1;
			kla_items[c] = rand()%num_itemtypes;
			if (itemtypes[kla_items[c]].flag & 1)
				end = 0;
			if (itemtypes[kla_items[c]].type != item_system)
				end = 0;
			for (t = 0; t < c; t++)
				if (kla_items[t] == kla_items[c])
					end = 0;
			for (s = 0; s < num_stars; s++)
			if (s != homesystem && sm_stars[s].card > -1)
			{
				t = ecards[sm_stars[s].card].type;
				if (t == card_item || t == card_rareitem || t == card_lifeform)
				{
					if (ecards[sm_stars[s].card].parm == kla_items[c])
						end = 0;
				}
			}

		}
		c++;
	}

	for (; c < kla_numitems; c++)
	{
		end = 0;
		while (!end)
		{
			end = 1;
			kla_items[c] = rand()%num_itemtypes;
			if (itemtypes[kla_items[c]].flag & 1)
				end = 0;
			for (t = 0; t < c; t++)
				if (kla_items[t] == kla_items[c])
					end = 0;
			for (s = 0; s < num_stars; s++)
			if (s != homesystem && sm_stars[s].card > -1)
			{
				t = ecards[sm_stars[s].card].type;
				if (t == card_item || t == card_rareitem || t == card_lifeform)
				{
					if (ecards[sm_stars[s].card].parm == kla_items[c])
						end = 0;
				}
			}

		}
	}

}

void starmap_initshipnames()
{
	FILE* ini;
	char s1[64];
	char end;
	int num;
	int flag;
	int n;

	ini = myopen("gamedata/names.ini", "rb");
	if (!ini)
		return;

	num_captnames = 0;
	num_shipnames = 0;
	end = 0; num = 0; 
	flag = 0; 
	while (!end)
	{
		end = read_line1(ini, s1);
		if (!strcmp(s1, item_keywords[itkBegin]))
			num++;
		if (!strcmp(s1, "CAPTNAMES"))
		{	flag = 1; n=0; }
		else if (!strcmp(s1, "SHIPNAMES"))
		{ flag = 2; n=0; }
		else if (flag>0 && strcmp(s1, "END")==0)
			flag = 0;
		else if (flag == 1)
		{
			strcpy(captnames[num_captnames++], s1);
		}
		else if (flag == 2)
		{
			strcpy(shipnames[num_shipnames++], s1);
		}

	}
	fclose(ini);
	
}

void starmap_initracefleets()
{
	FILE* ini;
	char s1[64], s2[256];
	char end;
	int num;
	int flag;
	int n, com;
	int tv1, tv2, tv3;

	ini = myopen("gamedata/fleets.ini", "rb");
	if (!ini)
		return;

	end = 0; num = 0; 
	flag = 0; 
	while (!end)
	{
		end = read_line(ini, s1, s2);
		if (!strcmp(s1, item_keywords[rflBegin]))
			num++;
	}
	fclose(ini);
/*
	racefleets = (t_racefleet*)calloc(num, sizeof(t_racefleet));
	if (!racefleets)
		return;
	*/
	num_racefleets = num;

	ini = myopen("gamedata/fleets.ini", "rb");

	end = 0; num = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		com = -1;
		for (n = 0; n < rflMax; n++)
			if (!strcmp(s1, raceflt_keywords[n]))
				com = n;

		if (flag == 0)
		{
			if (com == rflBegin)
			{
				racefleets[num].race=-1;
				racefleets[num].num_fleets=0;
				flag = 1;
			}
		}
		else switch(com)
		{
			case rflRace:
			for (n = 0; n < num_races; n++)
				if (!strcmp(s2, races[n].name))
				{	racefleets[num].race = n; races[n].fleet = num; }
			break;

			case rflShip1:
			case rflShip2:
			case rflShip3:
			for (n = 0; n < num_shiptypes; n++)
				if (!strcmp(s2, shiptypes[n].name))
					racefleets[num].stype[com-rflShip1] = n;
			break;

			case rflFleet:
			sscanf(s2, "%d %d %d", &tv1, &tv2, &tv3);
			racefleets[num].fleets[racefleets[num].num_fleets][0]=tv1;
			racefleets[num].fleets[racefleets[num].num_fleets][1]=tv2;
			racefleets[num].fleets[racefleets[num].num_fleets][2]=tv3;
			racefleets[num].num_fleets++;
			break;

			case rflEasy:
			case rflMedium:
			case rflHard:
			for (n = 0; n < 10; n++)
			{
				racefleets[num].diff[com-rflEasy][n]=s2[n]-'0';
			}
			break;

			case rflEnd:
			num++; flag = 0;
			break;

			default: ;
		}

	}
	fclose(ini);
}

void starmap_deinitracefleets()
{
	num_racefleets = 0;
//	free(racefleets);
}