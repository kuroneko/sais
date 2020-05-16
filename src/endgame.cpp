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
#include "starmap.h"

#include "endgame.h"

// ----------------
//     CONSTANTS
// ----------------

// ----------------
// GLOBAL VARIABLES
// ----------------

t_job		*jobs;
int32		num_jobs;
int32    num_scores;
t_score  scores[20];
int32		 got_hiscore;

// ----------------
// LOCAL PROTOTYPES
// ----------------

void load_scores();
void save_scores();
void checkhiscore(int32 score);

// ----------------
// GLOBAL FUNCTIONS
// ----------------

void game_over()
{
	int32 end;
	int32 c, mc;
	int32 y, h;
	int32 job;
	int32 t;
	int32 val, v;
	int32 w;
	int32 bx, by;
	int32 mnv, mxv, vl[7];
	int32 dif = 12 + (settings.dif_enemies + settings.dif_nebula);
	t_ik_image *bg;
	char texty[512];
	char topic[64];
	char edate[32];
	char *bargraphnames[7] = 
	{
		textstring[STR_ENDGAME_BAR1],
		textstring[STR_ENDGAME_BAR2],
		textstring[STR_ENDGAME_BAR3],
		textstring[STR_ENDGAME_BAR4],
		textstring[STR_ENDGAME_BAR5],
		textstring[STR_ENDGAME_BAR6],
		textstring[STR_ENDGAME_BAR7],
	};
	t_ik_sprite *deathpic;

	bg = ik_load_pcx("graphics/starback.pcx", NULL);

	Stop_All_Sounds();

	prep_screen();
	ik_copybox(bg, screen, 0, 0, 640, 480, 0,0);

	y = 0;

	val = 0; 

#ifndef DEMO_VERSION
	vl[0] = -2000;
//	ik_print(screen, font_4x8, 8, y+=8, 0, "Initial loan repay: $%d", -2000);
	val -= 2000;

	c = player.stardate / 365;
	if (c >= 10)
	{
		c -= 9;
//		ik_print(screen, font_4x8, 8, y+=8, 0, "Time limit penalty: $%d", -1000*c);
		vl[1] = -c*1000;
		val -= c*1000;
	}
	else
		vl[1] = 0;
#else
	vl[0] = 0;
	vl[1] = 0;
#endif

	v = 0;
	for (c = 0; c < num_races; c++)
	{
		if (races[c].met)
			v += 300;
	}
//	ik_print(screen, font_4x8, 8, y+=8, 0, "Alien encounters..: $%d", v);
	v = (v * dif) / 16;
	val+=v;
	vl[2] = v;

	v = 0;
	for (c = 0; c < num_stars; c++)
	{
		if (sm_stars[c].explored && !sm_stars[c].novadate)
			v += platypes[sm_stars[c].planet].bonus;
	}
//	ik_print(screen, font_4x8, 8, y+=8, 0, "Planet exploration: $%d", v);
	v = (v * dif) / 16;
	val+=v;
	vl[3] = v;

	v = 0;
	for (c = 0; c < shiptypes[0].num_systems; c++)
	if (shipsystems[shiptypes[0].system[c]].item > -1)
	{
		v += itemtypes[shipsystems[shiptypes[0].system[c]].item].cost;
	}
//	ik_print(screen, font_4x8, 8, y+=8, 0, "Starship systems..: $%d", v);
	v = (v * dif) / 16;
	val+=v;
	vl[4] = v;

	v = 0;
	for (c = 0; c < player.num_items; c++)
	{
		v += itemtypes[player.items[c]].cost;
	}
//	ik_print(screen, font_4x8, 8, y+=8, 0, "Inventory items...: $%d", v);
	v = (v * dif) / 16;
	val+=v;
	vl[5] = v;

//	ik_print(screen, font_4x8, 8, y+=8, 0, "Other discoveries.: $%d", player.bonusdata);
	for (c = 0; c < STARMAP_MAX_FLEETS; c++)
	if (sm_fleets[c].num_ships>0 && sm_fleets[c].race!=race_klakar && sm_fleets[c].explored>0)
	{
		player.bonusdata += sm_fleets[c].explored*100;
	}
	if (player.num_ships>1)
	{
		player.bonusdata += 300*(player.num_ships-1);
	}

	//ik_print(screen, font_6x8, 0, 0, 1, "BD %d", player.bonusdata);

	v = player.bonusdata;
	v = (v * dif) / 16;
	val += v;
	vl[6] = v;

#ifdef DEMO_VERSION	// alternate time limit penalty
	c = player.stardate / 365;
	if (c >= 10)
	{
		c -= 9;
		vl[1] = -(val*c)/(c+1);
		val += vl[1];
	}
#endif

	mnv=0;mxv=0;
#ifndef DEMO_VERSION
	for (c = 0; c < 7; c++)
#else
	for (c = 1; c < 7; c++)
#endif
	{
		mnv = MIN(vl[c], mnv);
		mxv = MAX(vl[c], mxv);
	}

	bx = 288; by = 64; w = 160;
	v = bx+2 + (0-mnv)*(w-4)/(mxv-mnv);
#ifndef DEMO_VERSION
	for (c = 0; c < 7; c++)
#else
	for (c = 1; c < 7; c++)
#endif
	{
		ik_print(screen, font_4x8, 192, c*16+by, STARMAP_INTERFACE_COLOR, bargraphnames[c]);
		ik_drawbox(screen, bx, c*16+by, bx+w, c*16+by+8, STARMAP_INTERFACE_COLOR*16+10);
		ik_drawbox(screen, bx+1, c*16+by+1, bx+w-1, c*16+by+7, STARMAP_INTERFACE_COLOR*16+3);
		if (vl[c]<0)
			ik_drawbox(screen, bx+2 + (vl[c]-mnv)*(w-4)/(mxv-mnv), c*16+by+2, v, c*16+by+6, 24);
		else
			ik_drawbox(screen, v, c*16+by+2, bx+2 + (vl[c]-mnv)*(w-4)/(mxv-mnv), c*16+by+6, 72);
	}
	
	v = 0; 
	for (c = 0; c < 12; c++)
		if (player.stardate%365 >= months[c].sd)
			v = c;
	c = player.stardate%365 + 1 - months[v].sd;

	if (player.system == homesystem && player.death==0)
	{	bx = 192; by = 208; h = 128; }	// returns home
	else 
	{	bx = 192; by = 208; h = 128; } // dies

	sprintf(edate, textstring[STR_ENDGAME_DATEF], c, months[v].name, player.stardate/365+4580);

	sprintf(topic,  textstring[STR_ENDGAME_OVER], 
					textstring[STR_ENDGAME_DATE], edate);

	sprintf(edate, textstring[STR_ENDGAME_DATEF2], months[v].name, c, player.stardate/365+4580);

	if (player.system == homesystem && player.death==0) // return to hope
	{
		w = 0; h = 152 + (player.stardate>365*20)*8;
#ifndef DEMO_VERSION
		for (y = 0; y < STARMAP_MAX_FLEETS; y++)
		if (sm_fleets[y].race == race_kawangi && sm_fleets[y].num_ships > 0)	// kawangi left
		{
			w = 1; 
			val /= 2; 
			h = 208;
			break;
		}
#else
		if (val < 2) val = 2;
#endif

		interface_drawborder(screen, bx, by, bx+256, by+h, 1, STARMAP_INTERFACE_COLOR, topic);

		job = 0;
		for (y = 0; y < num_jobs; y++)
		if (val > jobs[y].value)
			job = y;

		y = 0;

		ik_dsprite(screen, bx+16, by+24, spr_SMraces->spr[RC_MUCRON], 0);
		ik_dsprite(screen, bx+16, by+24, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));

#ifndef DEMO_VERSION
		y += 1 + interface_textbox(screen, font_4x8, bx+84, by+24+y*8, 160, 128, 0, textstring[STR_ENDGAME_MUCRON1 + (player.stardate>365*20)]);
#else
		y += 1 + interface_textbox(screen, font_4x8, bx+84, by+24+y*8, 160, 128, 0, textstring[STR_ENDGAME_MUCRON1 + (player.stardate>=365*10)]);
#endif

		sprintf(texty, textstring[STR_ENDGAME_MUCRON3+2*(val<0)], abs(val));
		y += 1 + interface_textbox(screen, font_4x8, bx+84, by+24+y*8, 160, 128, 0, texty);
		y = MAX(9,y);

		sprintf(texty, textstring[STR_ENDGAME_MUCRON4+2*(val<0)], jobs[job].name);
		y += 1 + interface_textbox(screen, font_4x8, bx+16, by+24+y*8, 224, 128, 0, texty);

		if (!w)
		{
			ik_print(screen, font_4x8, bx+16, by+24+y*8, 0, textstring[STR_ENDGAME_MUCRON7]);
			Play_Sound(WAV_MUS_VICTORY, 15, 1);

			sprintf(player.deathmsg, "%s %s", textstring[STR_ENDGAME_MSG1], edate);
		}
#ifndef DEMO_VERSION
		else
		{
			ik_dsprite(screen, bx+16, by+24+y*8, spr_SMraces->spr[race_kawangi], 0);
			ik_dsprite(screen, bx+16, by+24+y*8, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));
			y += 1 + MAX(8,interface_textbox(screen, font_4x8, bx+84, by+24+y*8, 160, 128, 0, textstring[STR_ENDGAME_KAWANGI]));
			Play_Sound(WAV_KAWANGI, 15, 1);

			sprintf(player.deathmsg, "%s %s", textstring[STR_ENDGAME_MSG7], edate);
		}
#endif
	}
	else		// died
	{
		val /= 2;
#ifdef DEMO_VERSION
		if (val < 2) val = 2;
#endif
		switch (player.death)
		{
			default:
			case 1:	// combat
				sprintf(texty, 
								textstring[STR_ENDGAME_DEATH1], 
								shiptypes[0].name);
				sprintf(player.deathmsg, "%s %s", textstring[STR_ENDGAME_MSG2], edate);
				deathpic = spr_SMraces->spr[RC_BLOWUP];
			break;
			case 2:	// hole
				sprintf(texty,
								textstring[STR_ENDGAME_DEATH2],
								shiptypes[0].name);
				sprintf(player.deathmsg, "%s %s", textstring[STR_ENDGAME_MSG3], edate);			
				deathpic = spr_SMraces->spr[RC_HOLED];
				break;
			case 3: // vacuum collapse
				sprintf(texty,
								textstring[STR_ENDGAME_DEATH3],
								shiptypes[0].name);
				sprintf(player.deathmsg, "%s %s", textstring[STR_ENDGAME_MSG4], edate);			
				deathpic = spr_SMraces->spr[RC_BLOWUP];
			break;
			case 4: // nova shockwave
				sprintf(texty,
								textstring[STR_ENDGAME_DEATH4],
								shiptypes[0].name);
				sprintf(player.deathmsg, "%s %s", textstring[STR_ENDGAME_MSG5], edate);			
				deathpic = spr_SMraces->spr[RC_BLOWUP];
			break;
			case 5: // sabotage
				sprintf(texty,
								textstring[STR_ENDGAME_DEATH5],
								shiptypes[0].name);
				sprintf(player.deathmsg, "%s %s", textstring[STR_ENDGAME_MSG6], edate);
				deathpic = spr_SMraces->spr[RC_BLOWUP];
			break;
			case 6: // glory harvested by kawangi
				sprintf(texty,
								textstring[STR_ENDGAME_DEATH6],
								shiptypes[0].name);
				sprintf(player.deathmsg, "%s %s", textstring[STR_ENDGAME_MSG7], edate);
				deathpic = spr_SMplanet2->spr[22];
			break;
			case 7: // glory destroyed by vacuum collapser
				sprintf(texty,
								textstring[STR_ENDGAME_DEATH7],
								shiptypes[0].name);
				sprintf(player.deathmsg, "%s %s", textstring[STR_ENDGAME_MSG8], edate);
				deathpic = spr_SMraces->spr[RC_LOST];
			break;
 		}

		h = 56 + 8*MAX(8,interface_textboxsize(font_4x8, 160, 128, texty));

		interface_drawborder(screen, bx, by, bx+256, by+h, 1, STARMAP_INTERFACE_COLOR, topic);

		ik_dsprite(screen, bx+16, by+24, deathpic, 0);
		ik_dsprite(screen, bx+16, by+24, spr_IFborder->spr[18], 2+(STARMAP_INTERFACE_COLOR<<8));

		y = 0;
		y += 1 + interface_textbox(screen, font_4x8, bx+84, by+24+y*8, 160, 128, 0, texty);

		y = MAX(9, y);
		sprintf(texty, textstring[STR_ENDGAME_SCORE], val);
		y += 1 + interface_textbox(screen, font_4x8, bx+16, by+24+y*8, 224, 128, 0, texty);
		Play_Sound(WAV_MUS_DEATH, 15, 1);
	}

	ik_blit();

	update_palette();

	start_ik_timer(1, 20);
//	Play_Sound(WAV_MUS_TITLE, 15, 1, 0);

	end = 0;
	while (!end && !must_quit)
	{
		ik_eventhandler();
		v = t;
		t = get_ik_timer(1);
		c = ik_inkey();
		mc = ik_mclick();

		/*if (t > v)
		{	// half at 17.. full at 34
			if (t < 34*50)
				v = (100*t)/(34*50);
			else
				v = 100;
			Set_Sound_Volume(15, v);
		}*/

		if (mc==1 || c==13 || c==32)
			end = 1;

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ prep_screen(); ik_blit();	}

	}
	del_image(bg);
	Stop_All_Sounds();

	checkhiscore(val);
}

void checkhiscore(int32 score)
{
	int32 c;
	int32 b;

	Stop_All_Sounds();


	got_hiscore = -1;

	b = 0;
	if (num_scores>0)
	{
		while (b < 20 && b < num_scores && scores[b].score >= score)
			b++;
	}

	if (b < 20)
	{
		if (num_scores<20)
			num_scores++;
		for (c = num_scores-1; c > b; c--)
		{
			memcpy(&scores[c], &scores[c-1], sizeof(t_score));
		}
		strcpy(scores[b].cname, player.captname);
		strcpy(scores[b].sname, player.shipname);
		scores[b].score = score;
		scores[b].date = player.stardate;
		strcpy(scores[b].deathmsg, player.deathmsg);
		got_hiscore = b;
	}

	save_scores();
}

void endgame_init()
{
	FILE* ini;
	char s1[64], s2[256];
	char end;
	int num;
	int flag;
	int n;
	int tv1;
	
	ini = myopen("gamedata/jobs.ini", "rb");
	if (!ini)
		return;

	end = 0; num = 0; n = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		if (!strcmp(s1, "JOBS"))
			flag=1;
		else if (!strcmp(s1, "END"))
			flag = 0;
		else if (flag)
			num++;
	}
	fclose(ini);

	jobs = (t_job*)calloc(num, sizeof(t_job));
	if (!jobs)
		return;
	num_jobs = num;

	ini = myopen("gamedata/jobs.ini", "rb");
	end = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		if (!strcmp(s1, "JOBS"))
		{	flag = 1; num = 0; }
		else if (!strcmp(s1, "END"))
			flag = 0;
		else 
		{
			if (flag == 1)
			{
				sscanf(s1, "%d", &tv1);
				jobs[num].value = tv1;
				strcpy(jobs[num].name, s2);
				num++;
			}
		}
	}
	fclose(ini);

	load_scores();
}

void endgame_deinit()
{
	num_jobs = 0;
	free(jobs);

}

void load_scores()
{
	FILE* fil;
	char scorefile[256];
	sprintf(scorefile, "%s%s", moddir, "scores.dat");

	fil = myopen(scorefile, "rb");
	if (!fil)
	{
		num_scores = 0;
		return;
	}

	num_scores = fgetc(fil);
	fgetc(fil); fgetc(fil);	fgetc(fil);

	if (num_scores>20) num_scores=20;

	fread(scores, sizeof(t_score), num_scores, fil);
	fclose(fil);
}

void save_scores()
{
	FILE* fil;
	char scorefile[256];
	sprintf(scorefile, "%s%s", moddir, "scores.dat");

	fil = myopen(scorefile, "wb");
	if (!fil)
	{
		return;
	}

	fputc(num_scores, fil);
	fputc(0, fil); fputc(0, fil); fputc(0, fil);
	fwrite(scores, sizeof(t_score), num_scores, fil);
	fclose(fil);
}