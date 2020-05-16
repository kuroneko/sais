// ----------------
//     INCLUDES
// ----------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "typedefs.h"
#include "iface_globals.h"
#include "is_fileio.h"
#include "gfx.h"
#include "snd.h"
#include "interface.h"
#include "starmap.h"
#include "textstr.h"

#include "combat.h"

// ----------------
// GLOBAL FUNCTIONS
// ----------------

void combat_gethardpoint(t_ship *ship, int32 hdp, int32 *rx, int32 *ry)
{
	t_hull *hull;

	if (hdp == -1)	// if no hardpoint specified, hit center
	{
		*rx = ship->x;
		*ry = ship->y;
		return;
	}

	hull = &hulls[shiptypes[ship->type].hull];

	*rx = ship->x + ((( (hull->hardpts[hdp].x-32) * cos1k[ship->a] + 
				(32-hull->hardpts[hdp].y) * sin1k[ship->a] ) * hull->size) >> 12);
	*ry = ship->y + ((( (32-hull->hardpts[hdp].y) * cos1k[ship->a] + 
				(32-hull->hardpts[hdp].x) * sin1k[ship->a] ) * hull->size) >> 12);
}

int32 combat_findtarget(t_ship *ship, int32 hdp)
{
	int32 e;
	int32 s;
	int32 a1, a2, a3;
	int32 md, d;
	t_shipweapon *wep;

	s = shiptypes[ship->type].system[hdp];
	if (shipsystems[s].type != sys_weapon)
		return -1;
	if (shipsystems[s].par[0] == -1)	// empty slot
		return -1;
	wep = &shipweapons[shipsystems[s].par[0]];

	md = wep->range;
	e = -1;

	for (s = 0; s < MAX_COMBAT_SHIPS; s++)
	if (&cships[s] != ship && cships[s].type>-1 && (cships[s].own&1) != (ship->own&1) && 
			cships[s].hits>0 && cships[s].active==2 && cships[s].cloaked==0)
	{
		a1 = get_direction( (cships[s].x>>10)-(ship->x>>10), (cships[s].y>>10)-(ship->y>>10) );
		d = get_distance( (cships[s].x>>10)-(ship->x>>10), (cships[s].y>>10)-(ship->y>>10) );
		a2 = (ship->a + hulls[shiptypes[ship->type].hull].hardpts[hdp].a) & 1023;
		if (d < md || (s == ship->target && d < wep->range))
		{
			a3 = a1 - a2;
			while (a3 > 512) a3-=1024;
			while (a3 <-512) a3+=1024;
			a3 = ABS(a3);
			if (a3 < hulls[shiptypes[ship->type].hull].hardpts[hdp].f)
			{
				e = s;
				md = d;
				if (s == ship->target)
				{ md = 0;	break; }
			}
		}
	}

	return e;
}

void combat_fire(t_ship *src, int32 hdp, t_ship *trg, int32 start)
{
	t_shipweapon *wep;
	int s = shiptypes[src->type].system[hdp];
	if (shipsystems[s].type != sys_weapon)
		return;
	if (src->syshits[hdp]<1)
		return;
	wep = &shipweapons[shipsystems[s].par[0]];
//	t_shipweapon *wep = &shipweapons[shiptypes[src->type].weapon[hdp]];

	if (wep->type == 0)
	{
		combat_addbeam(wep, src, hdp, trg, start);
	}
	else
	{
		combat_addproj(src, hdp, trg, start);
	}
}

int32 combat_addbeam(t_shipweapon *wep, t_ship *src, int32 hdp, t_ship *trg, int32 start, int32 stg)
{
	int32 c;
	int32 b;
	int32 cpu;
	int32 hit;
	int s = shiptypes[src->type].system[hdp];

	if (stg == -1)
	{
		if (shipsystems[s].type != sys_weapon)
			return -1;
		wep = &shipweapons[shipsystems[s].par[0]];
	//	t_shipweapon *wep = &shipweapons[shiptypes[src->type].weapon[hdp]];

		cpu = src->cpu_type;
		hit = (40*(cpu+1)*100) / (trg->speed+100);
	}
	else
		hit = 100;

	b = -1;
	for (c = 0; c < MAX_COMBAT_BEAMS; c++)
	if (!cbeams[c].wep)
	{ b = c; break; }

	if (b==-1)
		return -1;

	cbeams[b].wep = wep;
	cbeams[b].stg = stg;

	cbeams[b].src = src;
	cbeams[b].stp = hdp;
	if (rand()%100 < hit)
	{
		cbeams[b].dst = trg;
		cbeams[b].dsp = -1;
	}
	else
	{
		cbeams[b].dst = NULL;
		cbeams[b].ang = get_direction( trg->x - src->x, trg->y - src->y );
		cbeams[b].len = wep->range;
	}
	cbeams[b].str = start;
	cbeams[b].dmt = start + wep->speed * 2;
	cbeams[b].end = start + wep->speed * 5;

	if (stg == -1)
	{
		if (src->syshits[hdp]<5) // damaged
			src->wepfire[hdp] = start + wep->rate * (3 + 2*(src->cpu_type<3)) * (1+rand()%3);
		else
			src->wepfire[hdp] = start + wep->rate * (3 + 2*(src->cpu_type<3));
	}

	combat_SoundFX(wep->sound1, src->x); 
	return b;
}

int32 calc_leadangle(int32 tx, int32 ty, int32 vtx, int32 vty, 
										 int32 sx, int32 sy, int32 vsx, int32 vsy,
										 int32 speed)
{
	int32 a1, a2, a3;
	int32 r;
	int32 a;
	double sinb;

	a1 = get_direction( (tx>>10)-(sx>>10), (ty>>10)-(sy>>10) );
	a2 = get_direction( (vtx - vsx), (vty - vsy) );
	r = (get_distance( (vtx - vsx), (vty - vsy) ) * COMBAT_FRAMERATE) >> 10;		// target speed
	a3 = (a2 - a1 + 1536) & 1023; 
	if (a3 > 512) a3-=1024;
	sinb = r * sin (a3 * 3.14159 / 512) / speed;
	if (sinb > 1)
		return -1;
	a = (int32)(asin(sinb) * 512 / 3.14159);
	a = (a1 - a + 1024) & 1023;

	return a;
}

int32 combat_addproj(t_ship *src, int32 hdp, t_ship *trg, int32 start)
{
	int32 c;
	int32 b;
	int32 a;
	int32 a1, a2, a3;
	int32 r;
	int32 eta;
	int32 sx, sy;
	int32 tx, ty;
	double sinb;
	t_hull *hull;
	int32 cpu;
	int s;
	t_shipweapon *wep;
	
	s = shiptypes[src->type].system[hdp];
	if (shipsystems[s].type != sys_weapon)
		return -1;
	wep = &shipweapons[shipsystems[s].par[0]];
//	t_shipweapon *wep = &shipweapons[shiptypes[src->type].weapon[hdp]];

	b = -1;
	for (c = 0; c < MAX_COMBAT_PROJECTILES; c++)
	if (!cprojs[c].wep)
	{ b = c; break; }

	if (b==-1)
		return -1;

	cpu = src->cpu_type;
	hull = &hulls[shiptypes[src->type].hull];
	combat_gethardpoint(src, hdp, &sx, &sy);
	combat_gethardpoint(trg, -1, &tx, &ty);
	r = get_distance( (tx>>10)-(sx>>10), (ty>>10)-(sy>>10) );
	eta = (r * COMBAT_FRAMERATE) / wep->speed;

	if (cpu == 0 || (wep->flags & wpfHoming))
		a = get_direction( (tx>>10)-(sx>>10), (ty>>10)-(sy>>10) );
	else if (cpu == 1)
	{
//		tx = tx + (trg->vx - src->vx) * eta;
//		ty = ty + (trg->vy - src->vy) * eta;
		for (c = 0; c < 3; c++)
		{
			tx = trg->x + (trg->vx - src->vx) * eta;
			ty = trg->y + (trg->vy - src->vy) * eta;
			r = get_distance( (tx>>10)-(sx>>10), (ty>>10)-(sy>>10) );
			eta = (r * COMBAT_FRAMERATE) / wep->speed;
		}
		a = get_direction( (tx>>10)-(sx>>10), (ty>>10)-(sy>>10) );
	}
	else if (cpu >= 2)
	{
		a1 = get_direction( (tx>>10)-(sx>>10), (ty>>10)-(sy>>10) );
		a2 = get_direction( (trg->vx - src->vx), (trg->vy - src->vy) );
		r = (get_distance( (trg->vx - src->vx), (trg->vy - src->vy) ) * COMBAT_FRAMERATE) >> 10;		// target speed
		a3 = (a2 - a1 + 1536) & 1023; 
		if (a3 > 512) a3-=1024;
		sinb = r * sin (a3 * 3.14159 / 512) / wep->speed;
		if (sinb > 1)
			return -1;
		a = (int32)(asin(sinb) * 512 / 3.14159);

		//ty = get_distance( (tx>>10)-(sx>>10), (ty>>10)-(sy>>10) );
		tx = (cos1k[(abs(a3)+1024) & 1023]*r + cos1k[(abs(a)+1024)&1023]*wep->speed) / wep->speed;
		if (tx > 0)
		eta = (eta * 65536) / tx;

#ifdef DEBUG_COMBAT
		sprintf(combatdebug, "ETA %d  TX %d  A %d  A3 %d", eta, tx, a, a3);
#endif

		a = (a1 - a + 1024) & 1023;



		/*for (c = 0; c < 5; c++)
		{
			tx = trg->x + (trg->vx - src->vx) * eta;
			ty = trg->y + (trg->vy - src->vy) * eta;
			r = get_distance( (tx>>10)-(sx>>10), (ty>>10)-(sy>>10) );
			eta = (r * COMBAT_FRAMERATE) / wep->speed;
		}*/
	}

	if (wep->flags && wpfImplode)
		a = (1024 + a + rand()%30 - 15) & 1023;
	
//	a = (src->a + hull->hardpts[hdp].a + 1024) & 1023;

	cprojs[b].x = sx;
	cprojs[b].y = sy;
	cprojs[b].vx = src->vx + ((sin1k[a] * wep->speed / COMBAT_FRAMERATE) >> 6);
	cprojs[b].vy = src->vy + ((cos1k[a] * wep->speed / COMBAT_FRAMERATE) >> 6);
	cprojs[b].a = get_direction ( cprojs[b].vx, cprojs[b].vy );

	cprojs[b].wep = wep;
	cprojs[b].src = src;
	cprojs[b].dst = trg;
	cprojs[b].str = start;

	if (wep->flags & wpfSpin)
		cprojs[b].va = 20;
	else
		cprojs[b].va = 0;

	if (wep->flags & wpfNova)
	{
		combat_addexplo(cprojs[b].x + cprojs[b].vx * 4, 
										cprojs[b].y + cprojs[b].vy * 4, 
										spr_shockwave, 1, 64, 0, start, start+6, 4);
	}

	if (wep->flags & wpfHoming)
	{
		cprojs[b].end = start + COMBAT_FRAMERATE * (wep->range * 3/2) / wep->speed;

		if (cprojs[b].dst->ecm_type > -1 && cprojs[b].dst->syshits[cprojs[b].dst->sys_ecm]>0)
		{	// ecm
			a = shipsystems[cprojs[b].dst->ecm_type].par[0] * 10;
			if (rand()%30 < a)
			{
				//Play_SoundFX(WAV_SYSFIXED);
				cprojs[b].dst = NULL;
				cprojs[b].va = (rand()%4) + 1;
				if (rand()&1) cprojs[b].va = -cprojs[b].va;
			}
		}
	}
	else
	{
		cprojs[b].end = start + COMBAT_FRAMERATE * wep->range / wep->speed;
		if (wep->flags & wpfSplit)
		{
			cprojs[b].end = start + eta - (COMBAT_FRAMERATE * 100) / wep->speed;
			if (cprojs[b].end < start + 10)
				cprojs[b].end = start + 10;
		}
	}
	
	if (wep->flags & wpfImplode)
		cprojs[b].end = start + COMBAT_FRAMERATE * (wep->range * 3/2) / wep->speed;

	if (wep->flags & wpfDisperse)
		cprojs[b].hits = wep->damage;
	else
		cprojs[b].hits = 1;

	if (src->syshits[hdp]<5) // damaged
		src->wepfire[hdp] = start + wep->rate * (3 + 2*(src->cpu_type<3)) * (1+rand()%3);
	else
		src->wepfire[hdp] = start + wep->rate * (3 + 2*(src->cpu_type<3));

	combat_SoundFX(wep->sound1, cprojs[b].x); 
	return b;
}

void combat_launchstages(int32 p, int32 num, int32 start)
{
	int32 c;
	int32 b;
	int32 a;
	int32 f;
	int32 sx, sy;
	int32 tx, ty;
	int32 n;
	t_ship *trg;

	t_shipweapon *wep = &shipweapons[cprojs[p].wep->stage];
	if (cprojs[p].wep->flags & wpfHoming)
	{
		if (cprojs[p].dst)
		{
			trg = cprojs[p].dst;
		}
		else
			return;
	}
	else
		trg = cprojs[p].dst;

	combat_SoundFX(wep->sound1, cprojs[p].x); 
	for (n = 0; n < num; n++)
	{
		b = -1;
		for (c = 0; c < MAX_COMBAT_PROJECTILES; c++)
		if (!cprojs[c].wep)
		{ b = c; break; }

		if (b==-1)
			break;

		sx = cprojs[p].x; 
		sy = cprojs[p].y;
		combat_gethardpoint(trg, -1, &tx, &ty);

		f = 0;
		if (cprojs[p].wep->item != -1)
			if (shipsystems[itemtypes[cprojs[p].wep->item].index].par[2])
				f = shipsystems[itemtypes[cprojs[p].wep->item].index].par[2];

		if (f & 1)
			a = rand()&1023;
		else
		{
			if (cprojs[p].wep->flags & wpfHoming)
				a = get_direction( (tx>>10)-(sx>>10), (ty>>10)-(sy>>10) );
			else
				a = cprojs[p].a;
			if (num>1)
				a = (a + 768 + (512 * n) / (num-1)) & 1023;
		}

		cprojs[b].x = sx;
		cprojs[b].y = sy;
		if (f & 2)
		{
			cprojs[b].vx = ((sin1k[a] * wep->speed / COMBAT_FRAMERATE) >> 6);
			cprojs[b].vy = ((cos1k[a] * wep->speed / COMBAT_FRAMERATE) >> 6);
		}
		else
		{
			cprojs[b].vx = cprojs[p].vx + ((sin1k[a] * wep->speed / COMBAT_FRAMERATE) >> 6);
			cprojs[b].vy = cprojs[p].vy + ((cos1k[a] * wep->speed / COMBAT_FRAMERATE) >> 6);
		}
		cprojs[b].a = get_direction(cprojs[b].vx, cprojs[b].vy);

		cprojs[b].wep = wep;
		cprojs[b].src = cprojs[p].src;
		cprojs[b].dst = trg;
		cprojs[b].str = start;
		if (wep->flags & wpfHoming)
		{
			cprojs[b].end = start + COMBAT_FRAMERATE * (wep->range * 3/2) / wep->speed;
			if (cprojs[b].dst->ecm_type > -1 && cprojs[b].dst->syshits[cprojs[b].dst->sys_ecm]>0)
			{	// ecm
				a = shipsystems[cprojs[b].dst->ecm_type].par[0] * 10;
				if (rand()%30 < a)
				{
					Play_SoundFX(WAV_SYSFIXED, get_ik_timer(1));
					cprojs[b].dst = NULL;
					cprojs[b].va = (rand()%5 + 4)*((rand()&1)*2-1);
				}
			}
		}
		else
			cprojs[b].end = start + COMBAT_FRAMERATE * wep->range / wep->speed;

		if (wep->flags & wpfSpin)
			cprojs[b].va = 20;
		else
			cprojs[b].va = 0;

		if (wep->flags & wpfDisperse)
			cprojs[b].hits = wep->damage;
		else
			cprojs[b].hits = 0;
	}
}

void combat_damageship(int32 s, int32 src, int32 dmg, int32 t, t_shipweapon *wep, int32 deb)
{
	int32 sys = -1;
	int32 d1;
	int32 c;

	if (s == -1)
		return;

	if (cships[s].type == -1)	// now this would suck
		return;

	if (t > cships[s].damage_time + 10)
	{
		if (cships[s].shld_type>-1 && cships[s].shld>0)
		{
			combat_SoundFX(WAV_SHIELD, cships[s].x); 
		}
		else
		{
			if (wep->type==0) // beam
				combat_SoundFX(WAV_EXPLO1, cships[s].x); 
			else
				combat_SoundFX(wep->sound2, cships[s].x); 
		}
		cships[s].damage_time = t;
	}

	if (cships[s].hits <= 0 || cships[s].active<2)
		return;

	if (!deb)
	{
		if (cships[s].shld_type>-1 && cships[s].shld>0)
		{
			if ( (rand()%10)==0)
				sys = cships[s].sys_shld;
			else
			{
				d1 = MIN(dmg, cships[s].shld);
				dmg = dmg - d1;
				cships[s].shld -= d1;
				cships[s].shld_time = t;
				if (cships[s].shld <= 0) cships[s].shld = 0;
			}
		}

		if (dmg>0)
		{
			if ( (rand()%10)<5 )
			{	
				sys = rand()%shiptypes[cships[s].type].num_systems;
				if (cships[s].syshits[sys]<=0 || shipsystems[shiptypes[cships[s].type].system[sys]].item==-1)
					sys = -1;
			}

			if (sys==-1)
				cships[s].hits -= dmg;	// hull hit
			else
			{	
				d1 = cships[s].syshits[sys];
				cships[s].syshits[sys] = MAX (0, cships[s].syshits[sys]-dmg); 
				if (cships[s].syshits[sys]>0)
				{
					if (cships[s].syshits[sys]/5 != d1/5)	// green->yellow, yellow->red
						combat_SoundFX(WAV_SYSHIT1+(rand()&1), cships[s].x);
				}
				else	// red->grey
					combat_SoundFX(WAV_SYSDAMAGE, cships[s].x);
			}	// system damage
		}
	}
	else
	{
		cships[s].hits -= dmg;
	}

	combat_updateshipstats(s, t);

	if (cships[s].hits <= 0) // burn
	{
		cships[s].va = rand()%cships[s].turn+1;
		if (rand()&1)
			cships[s].va = -cships[s].va;
		if (shiptypes[cships[s].type].race == race_unknown)	// stop space hulk
			cships[s].vx = cships[s].vy = 0;

		// reset other ships' targets 
		for (c = 0; c < MAX_COMBAT_SHIPS; c++)
		if (cships[c].type > -1 && cships[c].hits > 0)
		{
			if (cships[c].target == s)
			{
				cships[c].target = -1;
				combat_findstuff2do(c,t);
			}
		}
	}
}

void combat_killship(int32 s, int32 t, int32 quiet)
{
	int32 c;
	int32 sz;
	int32 shu=0;

	if (s == -1)
		return;
	if (cships[s].type == -1)
		return;

	if (shiptypes[cships[s].type].flag & 256)
		shu = 1;

	if (!quiet)
		cships[s].hits = -666;
	sz = hulls[shiptypes[cships[s].type].hull].size * 2;

	if (camera.ship_sel == s)
		camera.ship_sel = -1;

	for (c = 0; c < MAX_COMBAT_SHIPS; c++)
	if (cships[c].type > -1)
	{
		if (cships[c].target == s)
		{
			cships[c].target = -1;
			combat_findstuff2do(c,t);
		}
	}

	for (c = 0; c < MAX_COMBAT_PROJECTILES; c++)
	if (cprojs[c].wep != NULL)
	{
		if (cprojs[c].wep->flags & wpfHoming)
			if (cprojs[c].dst == &cships[s])
				cprojs[c].dst = NULL;
	}

	for (c = 0; c < MAX_COMBAT_BEAMS; c++)
	if (cbeams[c].wep != NULL)
	{
		if (cbeams[c].dst == &cships[s])
			cbeams[c].wep = NULL;
		if (cbeams[c].src == &cships[s])
			cbeams[c].wep = NULL;
	}


	if (shu)
	{
		cships[s].active = 1;
		cships[s].hits = 1;
	}
	else
	{
		cships[s].active = 0;
		if (!quiet)
		{
			combat_SoundFX(WAV_EXPLO2, cships[s].x);
			combat_addexplo(cships[s].x, cships[s].y, spr_shockwave, 
									5, sz*4, 1, t, t+sz/2, 0);
			combat_addexplo(cships[s].x, cships[s].y, spr_explode1, 
									5, sz, 0, t, t+sz/4);
		}
		cships[s].type = -1;
	}
}

int32 combat_addexplo(int32 x, int32 y, t_ik_spritepak *spr, int32 spin, int32 size, int32 zoom, int32 start, int32 end, int32 anim, int32 cam)
{
	int b, c;

	b = -1;
	for (c = 0; c < MAX_COMBAT_EXPLOS; c++)
	if (!cexplo[c].spr)
	{ b = c; break; }

	if (b == -1)
		return -1;

	cexplo[b].x = x;
	cexplo[b].y = y;
	cexplo[c].a = rand()%1024,
	cexplo[b].spr = spr;
	cexplo[b].str = start;
	cexplo[b].end = end;
	cexplo[b].size = size;
	cexplo[b].zoom = zoom;
	cexplo[b].va = spin;
	cexplo[b].anim = anim;
	cexplo[b].cam = cam;

	return b;
}
