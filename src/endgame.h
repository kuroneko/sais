/* Strange Adventures in Infinite Space
 * Copyright (C) 2005 Richard Carlson, Iikka Keranen and William Sears
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef SAIS_ENDGAME_H
#define SAIS_ENDGAME_H

#ifdef __cplusplus
extern "C" {
#endif

// ----------------
//    CONSTANTS
// ----------------

// ----------------
//     TYPEDEFS
// ----------------

typedef struct _t_job
{
	char name[64];	
	int32 value;
} t_job;

typedef struct _t_score
{
	char cname[16], sname[16];
	char deathmsg[64];
	int32 score;
	int32 date;
} t_score;

// ----------------
// GLOBAL VARIABLES
// ----------------

extern t_job		*jobs;
extern int32		num_jobs;

extern int32    num_scores;
extern t_score  scores[20];

extern int32		got_hiscore;

// ----------------
//    PROTOTYPES
// ----------------

void game_over();

void endgame_init();
void endgame_deinit();

#ifdef __cplusplus
};
#endif

#endif /* #ifndef SAIS_ENDGAME_H */
