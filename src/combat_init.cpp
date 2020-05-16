// ----------------
//     INCLUDES
// ----------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "typedefs.h"
#include "iface_globals.h"
#include "is_fileio.h"
#include "gfx.h"
#include "snd.h"
#include "starmap.h"
#include "combat.h"

// ----------------
//     CONSTANTS
// ----------------

char *hull_keywords[hlkMax] =
{
	"HULL",
	"NAME",
	"SIZE",
	"HITS",
	"MASS",
	"SPRI",
	"SILU",
	"WEAP",
	"ENGN",
	"THRU",
	"FTRB",
	"END",
};

char *shiptype_keywords[shkMax] =
{
	"STYP",
	"NAME",
	"RACE",
	"FLAG",
	"HULL",
	"ARMR",
	"SYST",
	"ENGN",
	"THRU",
	"WEAP",
	"END",
};

char *shipweapon_keywords[wpkMax] = 
{
	"WEAP",
	"NAME",
	"STGE",
	"TYPE",
	"FLAG",
	"SPRI",
	"SIZE",
	"SND1",
	"SND2",
	"RATE",
	"SPED",
	"DAMG",
	"RANG",
	"END",
};

char *shipweapon_flagwords[wpfMax] =
{
	"trans",
	"spin",
	"disperse",
	"implode",
	"homing",
	"split",
	"shock1",
	"shock2",
	"nova",
	"wiggle",
	"strail",
	"noclip",
};

char *shipsystem_keywords[sykMax] = 
{
	"SYST",
	"NAME",
	"TYPE",
	"SIZE",
	"PAR1",
	"PAR2",
	"PAR3",
	"PAR4",
	"END",
};

char *race_keywords[rckMax] =
{
	"RACE",
	"NAME",
	"TEXT",
	"TXT2",
	"END",
};

// ----------------
// GLOBAL VARIABLES
// ----------------

t_ik_image			*combatbg1;
t_ik_image			*combatbg2;

t_ik_spritepak	*spr_ships;
t_ik_spritepak	*spr_shipsilu;
t_ik_spritepak	*spr_weapons;
t_ik_spritepak	*spr_explode1;
t_ik_spritepak	*spr_shockwave;
t_ik_spritepak	*spr_shield;

t_hull					*hulls;
int							num_hulls;

t_shiptype			*shiptypes;
int							num_shiptypes;

t_shipweapon		*shipweapons;
int							num_shipweapons;

t_shipsystem		*shipsystems;
int							num_shipsystems;

//char						racename[16][32];
//int							num_races;
int							enemies[16];
int							num_enemies;

// ----------------
// LOCAL PROTOTYPES
// ----------------

void initraces();

void combat_inithulls();
void combat_deinithulls();

void combat_initshiptypes();
void combat_deinitshiptypes();

void combat_initshipweapons();
void combat_deinitshipweapons();

void combat_initshipsystems();
void combat_deinitshipsystems();

void combat_initsprites();
void combat_deinitsprites();

// ----------------
// GLOBAL FUNCTIONS
// ----------------

void combat_init()
{
	initraces();
	combat_initsprites();
	combat_initshipweapons();
	combat_initshipsystems();
	combat_inithulls();
	combat_initshiptypes();
}

void combat_deinit()
{
	combat_deinitshiptypes();
	combat_deinithulls();
	combat_deinitshipweapons();
	combat_deinitshipsystems();
	combat_deinitsprites();
}

// ----------------
// LOCAL FUNCTIONS
// ----------------

void combat_inithulls()
{
	FILE* ini;
	char s1[64], s2[256];
	char end;
	int num;
	int flag;
	int n, com;

	char ts1[64];
	int tv1, tv2, tv3, tv4, tv5;

	ini = myopen("gamedata/hulls.ini", "rb");
	if (!ini)
		return;

	end = 0; num = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		if (!strcmp(s1, hull_keywords[hlkBegin]))
			num++;
	}
	fclose(ini);

	hulls = (t_hull*)calloc(num, sizeof(t_hull));
	if (!hulls)
		return;
	num_hulls = num;

	ini = myopen("gamedata/hulls.ini", "rb");

	end = 0; num = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		com = -1;
		for (n = 0; n < hlkMax; n++)
			if (!strcmp(s1, hull_keywords[n]))
				com = n;

		if (flag == 0)
		{
			if (com == hlkBegin)
			{
				hulls[num].numh = 0;
				flag = 1;
			}
		}
		else switch(com)
		{
			case hlkName:
			strcpy(hulls[num].name, s2);
			break;

			case hlkSize:
			sscanf(s2, "%d", &tv1);
			hulls[num].size = tv1;
			break;

			case hlkHits:
			sscanf(s2, "%d", &tv1);
			hulls[num].hits = tv1;
			break;

			case hlkMass:
			sscanf(s2, "%d", &tv1);
			hulls[num].mass = tv1;
			break;

			case hlkSprite:
			sscanf(s2, "%s %d", ts1, &tv2);
			hulls[num].sprite = spr_ships->spr[tv2];
			break;

			case hlkSilu:
			sscanf(s2, "%s %d", ts1, &tv2);
			hulls[num].silu = spr_shipsilu->spr[tv2];
			break;

			case hlkWeapon:
			case hlkEngine:
			case hlkThruster:
			case hlkFighter:
			sscanf(s2, "%d %d %d %d %d", &tv1, &tv2, &tv3, &tv4, &tv5);
			hulls[num].hardpts[hulls[num].numh].type = hdpWeapon + (com - hlkWeapon);
			hulls[num].hardpts[hulls[num].numh].x = tv1;
			hulls[num].hardpts[hulls[num].numh].y = tv2;
			hulls[num].hardpts[hulls[num].numh].a = (tv3 * 1024) / 360;
			hulls[num].hardpts[hulls[num].numh].f = (tv4 * 1024) / 360;
			hulls[num].hardpts[hulls[num].numh].size = tv5;
			hulls[num].numh++;
			break;

			case hlkEnd:
			num++; flag = 0;
			break;

			default: ;
		}

	}
	fclose(ini);
}

void combat_deinithulls()
{
	num_hulls = 0;
	free(hulls);
}

void combat_initshiptypes()
{
	FILE* ini;
	char s1[64], s2[256];
	char end;
	int num;
	int flag;
	int n, com;
	int wep;

	ini = myopen("gamedata/ships.ini", "rb");
	if (!ini)
		return;

	end = 0; num = 0; 
	flag = 0; num_enemies = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		if (!strcmp(s1, shiptype_keywords[shkBegin]))
			num++;
		if (!strcmp(s1, "ENEMIES"))
		{	flag = 2; n=0; }
		else if (flag>0 && strcmp(s1, "END")==0)
			flag = 0;
		else if (flag > 0)
		{
			if (flag == 2)
			{
				for (com = 0; com < num_races; com++)
					if (!strcmp(races[com].name, s2))
					{
						enemies[n]=com;
						num_enemies++;
						n++;
					}
			}
		}

	}
	fclose(ini);

	shiptypes = (t_shiptype*)calloc(num, sizeof(t_shiptype));
	if (!shiptypes)
		return;
	num_shiptypes = num;

	ini = myopen("gamedata/ships.ini", "rb");

	end = 0; num = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		com = -1;
		for (n = 0; n < shkMax; n++)
			if (!strcmp(s1, shiptype_keywords[n]))
				com = n;

		if (flag == 0)
		{
			if (com == shkBegin)
			{
				flag = 1;
				wep = 0;
				shiptypes[num].engine = -1;
				shiptypes[num].thrust = -1;
				shiptypes[num].num_systems = 0;
				shiptypes[num].flag = 0;
				for (n = 0; n < 16; n++)
					shiptypes[num].system[n] = -1;
			}
		}
		else switch(com)
		{
			case shkName:
			strcpy(shiptypes[num].name, s2);
			break;

			case shkRace:
			for (n = 0; n < num_races; n++)
				if (!strcmp(races[n].name, s2))
					shiptypes[num].race = n;
			break;

			case shkFlag:
			sscanf(s2, "%d", &n);
			shiptypes[num].flag = n;
			break;

			case shkHull:
			for (n = 0; n < num_hulls; n++)
				if (!strcmp(hulls[n].name, s2))
					shiptypes[num].hull = n;
			break;

			case shkSystem:
			case shkWeapon:
			case shkEngine:
			case shkThruster:
			for (n = 0; n < num_shipsystems; n++)
				if (!strcmp(shipsystems[n].name, s2))
				{	
					shiptypes[num].system[shiptypes[num].num_systems] = n;
					shiptypes[num].sysdmg[shiptypes[num].num_systems] = 0;
					shiptypes[num].num_systems++;
				}
			break;

			/*
			case shkEngine:
			for (n = 0; n < num_shipsystems; n++)
				if (!strcmp(shipsystems[n].name, s2))
					shiptypes[num].engine = n;
			break;

			case shkThruster:
			for (n = 0; n < num_shipsystems; n++)
				if (!strcmp(shipsystems[n].name, s2))
					shiptypes[num].thrust = n;
			shiptypes[num].speed = (shipsystems[shiptypes[num].thrust].par[0] * 32) / hulls[shiptypes[num].hull].mass;
			//shiptypes[num].speed = 1024 / hulls[shiptypes[num].hull].mass;
			break;

			case shkWeapon:
			for (n = 0; n < num_shipweapons; n++)
				if (!strcmp(shipweapons[n].name, s2))
					shiptypes[num].weapon[wep] = n;
			wep++;
			break;*/

			case shkEnd:
			sort_shiptype_systems(num);
			num++; flag = 0;
			break;

			default: ;
		}

	}
	fclose(ini);
}

void combat_deinitshiptypes()
{
	num_shiptypes = 0;
	free(shiptypes);
}

void combat_initshipweapons()
{
	FILE* ini;
	char s1[64], s2[256];
	char end;
	int num;
	int flag;
	int n, com;

	char ts1[64];
	char ts[4][64];
	int tv1, tv2;

	ini = myopen("gamedata/weapons.ini", "rb");
	if (!ini)
		return;

	end = 0; num = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		if (!strcmp(s1, shipweapon_keywords[wpkBegin]))
			num++;
	}
	fclose(ini);

	shipweapons = (t_shipweapon*)calloc(num, sizeof(t_shipweapon));
	if (!shipweapons)
		return;
	num_shipweapons = num;

	ini = myopen("gamedata/weapons.ini", "rb");

	end = 0; num = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		com = -1;
		for (n = 0; n < wpkMax; n++)
			if (!strcmp(s1, shipweapon_keywords[n]))
				com = n;

		if (flag == 0)
		{
			if (com == wpkBegin)
			{
				flag = 1;
				shipweapons[num].item = -1;
			}
		}
		else switch(com)
		{
			case wpkName:
			strcpy(shipweapons[num].name, s2);
			shipweapons[num].flags = 0;
			break;

			case wpkStage:
			for (n = 0; n < num; n++)
				if (!strcmp(shipweapons[n].name, s2))
					shipweapons[num].stage = n;
			break;

			case wpkType:
			sscanf(s2, "%d", &tv1);
			shipweapons[num].type = tv1;
			break;

			case wpkFlag:
			for (n = 0; n < 4; n++)
				strcpy(ts[n], "");
			sscanf(s2, "%s %s %s %s", ts[0], ts[1], ts[2], ts[3]);
			for (n = 0; n < 4; n++)
				for (tv1 = 0; tv1 < wpfMax; tv1++)
					if (!strcmp(shipweapon_flagwords[tv1], ts[n]))
						shipweapons[num].flags |= (1<<tv1);
			break;

			case wpkSprite:
			sscanf(s2, "%s %d", ts1, &tv2);
			shipweapons[num].sprite = spr_weapons->spr[tv2];
			break;

			case wpkSize:
			sscanf(s2, "%d", &tv1);
			shipweapons[num].size = tv1;
			break;

			case wpkSound1:
			sscanf(s2, "%d", &tv1);
			if (shipweapons[num].type==1)
				tv1+=SND_PROJS;
			else
				tv1+=SND_BEAMS;
			shipweapons[num].sound1 = tv1;
			break;

			case wpkSound2:
			sscanf(s2, "%d", &tv1);
			tv1+=SND_HITS;
			shipweapons[num].sound2 = tv1;
			break;

			case wpkRate:
			sscanf(s2, "%d", &tv1);
			shipweapons[num].rate = tv1;
			break;

			case wpkSpeed:
			sscanf(s2, "%d", &tv1);
			shipweapons[num].speed = tv1;
			break;

			case wpkDamage:
			sscanf(s2, "%d", &tv1);
			shipweapons[num].damage = tv1;
			break;

			case wpkRange:
			sscanf(s2, "%d", &tv1);
			shipweapons[num].range = tv1;
			break;

			case wpkEnd:
			num++; flag = 0;
			break;

			default: ;
		}

	}
	fclose(ini);
}

void combat_deinitshipweapons()
{
	num_shipweapons = 0;
	free(shipweapons);
}

void combat_initshipsystems()
{
	FILE* ini;
	char s1[64], s2[256];
	char end;
	int num;
	int flag;
	int n, com;
	int tv1;

	char systype[16][32];
	int32 num_systypes;

	ini = myopen("gamedata/systems.ini", "rb");
	if (!ini)
		return;

	end = 0; num = 0; 
	flag = 0; num_systypes = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		if (!strcmp(s1, shipsystem_keywords[sykBegin]))
			num++;
		if (!strcmp(s1, "SYSTEMTYPES"))
		{	flag = 1; n=0; }
		else if (flag>0 && strcmp(s1, "END")==0)
			flag = 0;
		else 	if (flag == 1)
		{
			strcpy(systype[n], s1);
			num_systypes++;
			n++;
		}

	}
	fclose(ini);

	shipsystems = (t_shipsystem*)calloc(num, sizeof(t_shipsystem));
	if (!shipsystems)
		return;
	num_shipsystems = num;

	ini = myopen("gamedata/systems.ini", "rb");

	end = 0; num = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		com = -1;
		for (n = 0; n < sykMax; n++)
			if (!strcmp(s1, shipsystem_keywords[n]))
				com = n;

		if (flag == 0)
		{
			if (com == sykBegin)
			{
				flag = 1;
				shipsystems[num].item = -1;
			}
		}
		else switch(com)
		{
			case sykName:
			strcpy(shipsystems[num].name, s2);
			break;

			case sykType:
			for (n = 0; n < num_systypes; n++)
				if (!strcmp(s2, systype[n]))
					shipsystems[num].type = n;
			if (shipsystems[num].type == sys_weapon)
			{
				shipsystems[num].par[0] = -1;
				for (n = 0; n < num_shipweapons; n++)
					if (!strcmp(shipsystems[num].name, shipweapons[n].name))
						shipsystems[num].par[0] = n;
			}
			break;

			case sykSize:
			sscanf(s2, "%d", &tv1);
			shipsystems[num].size = tv1;
			break;

			case sykParam1:
			case sykParam2:
			case sykParam3:
			case sykParam4:

			sscanf(s2, "%d", &tv1);
			shipsystems[num].par[com-sykParam1] = tv1;
			break;

			case sykEnd:
			num++; flag = 0;
			break;

			default: ;
		}

	}
	fclose(ini);
}

void combat_deinitshipsystems()
{
	num_shipsystems = 0;
	free(shipsystems);
}

void combat_initsprites()
{
	t_ik_image *pcx;	
	int x, y, n; 

	spr_ships = load_sprites("graphics/ships.spr");
	spr_shipsilu = load_sprites("graphics/shipsilu.spr");
	spr_weapons = load_sprites("graphics/weapons.spr");
	spr_explode1 = load_sprites("graphics/explode1.spr");
	spr_shockwave = load_sprites("graphics/shockwav.spr");
	spr_shield = load_sprites("graphics/shield.spr");

	combatbg1 = ik_load_pcx("graphics/combtbg1.pcx", NULL);
	combatbg2 = ik_load_pcx("graphics/combtbg2.pcx", NULL);

	if (!spr_ships)
	{
#ifndef DEMO_VERSION
		spr_ships = new_spritepak(24);
		pcx = ik_load_pcx("ships.pcx", NULL);
		for (n=0;n<24;n++)
#else
		spr_ships = new_spritepak(11);
		pcx = ik_load_pcx("../demogfx/ships.pcx", NULL);
		for (n=0;n<11;n++)
#endif
		{
			x = n%5; y = n/5;
			spr_ships->spr[n] = get_sprite(pcx, x*64, y*64, 64, 64);
		}

		del_image(pcx);
		save_sprites("graphics/ships.spr", spr_ships);
	}

	if (!spr_shipsilu)
	{
#ifndef DEMO_VERSION
		spr_shipsilu = new_spritepak(24);
		pcx = ik_load_pcx("silu.pcx", NULL);
		for (n=0;n<24;n++)
#else
		spr_shipsilu = new_spritepak(11);
		pcx = ik_load_pcx("../demogfx/silu.pcx", NULL);
		for (n=0;n<11;n++)
#endif
		{
			x = n%5; y = n/5;
			spr_shipsilu->spr[n] = get_sprite(pcx, x*128, y*128, 128, 128);
		}

		del_image(pcx);
		save_sprites("graphics/shipsilu.spr", spr_shipsilu);
	}

	if (!spr_weapons)
	{
		spr_weapons = new_spritepak(19);
		pcx = ik_load_pcx("weaponfx.pcx", NULL);

		for (n=0;n<5;n++)
		{
			spr_weapons->spr[n] = get_sprite(pcx, n*32, 0, 32, 32);
		}
		for (n=0;n<5;n++)
		{
			spr_weapons->spr[n+5] = get_sprite(pcx, n*32, 32, 32, 32);
		}
		for (n=0;n<4;n++)
		{
			spr_weapons->spr[n+10] = get_sprite(pcx, n*32, 64, 32, 32);
		}
		spr_weapons->spr[14] = get_sprite(pcx, 192, 64, 128, 128);
		spr_weapons->spr[15] = get_sprite(pcx, 0, 96, 32, 32);
		spr_weapons->spr[16] = get_sprite(pcx, 160, 0, 32, 32);
		spr_weapons->spr[17] = get_sprite(pcx, 128, 64, 32, 32); 
		spr_weapons->spr[18] = get_sprite(pcx, 32, 96, 32, 32); 

		del_image(pcx);
		save_sprites("graphics/weapons.spr", spr_weapons);
	}

	if (!spr_explode1)
	{
		spr_explode1 = new_spritepak(10);
		pcx = ik_load_pcx("xplosion.pcx", NULL);
		
		for (n=0; n<10; n++)
		{
			x = n%5; y = n/5;
			spr_explode1->spr[n] = get_sprite(pcx, x*64, y*64, 64, 64);
		}

		del_image(pcx);
		save_sprites("graphics/explode1.spr", spr_explode1);
	}

	if (!spr_shockwave)
	{
		spr_shockwave = new_spritepak(5);
		pcx = ik_load_pcx("shock.pcx", NULL);
		
		for (n=0; n<5; n++)
		{
			spr_shockwave->spr[n] = get_sprite(pcx, (n%3)*128, (n/3)*128, 128, 128);
		}

		del_image(pcx);
		save_sprites("graphics/shockwav.spr", spr_shockwave);
	}

	if (!spr_shield)
	{
		spr_shield = new_spritepak(5);
		pcx = ik_load_pcx("shields.pcx", NULL);
		
		for (n=0; n<5; n++)
		{
			spr_shield->spr[n] = get_sprite(pcx, n*128, 0, 128, 128);
		}

		del_image(pcx);
		save_sprites("graphics/shield.spr", spr_shield);
	}

}

void combat_deinitsprites()
{
	free_spritepak(spr_ships);
	free_spritepak(spr_shipsilu);
	free_spritepak(spr_weapons);
	free_spritepak(spr_explode1);
	free_spritepak(spr_shockwave);
	free_spritepak(spr_shield);

	del_image(combatbg1);
	del_image(combatbg2);
}

void initraces(void)
{
	FILE* ini;
	char s1[64], s2[256];
	char end;
	int num;
	int flag;
	int n, com;

	ini = myopen("gamedata/races.ini", "rb");
	if (!ini)
		return;

	end = 0; num = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		com = -1;
		for (n = 0; n < rckMax; n++)
			if (!strcmp(s1, race_keywords[n]))
				com = n;

		if (flag == 0)
		{
			if (com == rckBegin)
			{
				races[num].fleet=-1;
				flag = 1;
			}
		}
		else switch(com)
		{
			case rckName:
			strcpy(races[num].name, s2);
			break;

			case rckText:
			strcpy(races[num].text, s2);
			break;

			case rckText2:
			strcpy(races[num].text2, s2);
			break;

			case rckEnd:
			num++; flag = 0;
			break;

			default: ;
		}

	}
	num_races = num;
	fclose(ini);
}

void sort_shiptype_systems(int32 num)
{
	int n, c;
	int w, t;

	w = 0; 
	// systems are sorted by type (weapons first to match hardpoints)
	for (n=0; n < shiptypes[num].num_systems; n++)
	{
		c = n;
		while (c > 0 && shipsystems[shiptypes[num].system[c]].type < shipsystems[shiptypes[num].system[c-1]].type)
		{
			t = shiptypes[num].system[c];
			shiptypes[num].system[c] = shiptypes[num].system[c-1];
			shiptypes[num].system[c-1] = t;
			t = shiptypes[num].sysdmg[c];
			shiptypes[num].sysdmg[c] = shiptypes[num].sysdmg[c-1];
			shiptypes[num].sysdmg[c-1] = t;
			c--;
		}
	}

	shiptypes[num].engine = -1;
	shiptypes[num].thrust = -1;
	shiptypes[num].speed = 1;
	shiptypes[num].turn = 1;
	shiptypes[num].sensor = 0;
	for (n = 0; n < shiptypes[num].num_systems; n++)
	{
		if (shipsystems[shiptypes[num].system[n]].type == sys_thruster)
		{
			shiptypes[num].thrust = shiptypes[num].system[n];
			shiptypes[num].speed = (shipsystems[shiptypes[num].system[n]].par[0] * 32) / hulls[shiptypes[num].hull].mass;
			shiptypes[num].turn = (shipsystems[shiptypes[num].system[n]].par[0] * 3) / hulls[shiptypes[num].hull].mass + 1;
			shiptypes[num].sys_thru = n;
		}
		else if (shipsystems[shiptypes[num].system[n]].type == sys_engine)
		{	shiptypes[num].engine = shiptypes[num].system[n]; shiptypes[num].sys_eng = n; }
		else if (shipsystems[shiptypes[num].system[n]].type == sys_sensor)
			shiptypes[num].sensor = shipsystems[shiptypes[num].system[n]].par[0];
	}
}
