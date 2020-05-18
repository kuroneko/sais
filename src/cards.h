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
// ----------------
//     TYPEDEFS
// ----------------

typedef struct _t_eventcard
{
	char name[32];
	char text[256];
	char text2[256];
	int32 type;
	int32 parm;
} t_eventcard;

enum ecard_keyids
{
	eckBegin,
	eckName,
	eckText,
	eckText2,
	eckType,
	eckParam,
	eckEnd,
	eckMax,
};

enum ecard_types
{
	card_event,
	card_ally,
	card_item,
	card_rareitem,
	card_lifeform,
	card_max,
};

// ----------------
// GLOBAL VARIABLES
// ----------------

extern t_eventcard		*ecards;
extern int32					num_ecards;

// ----------------
//    PROTOTYPES
// ----------------

void cards_init();
void cards_deinit();
void card_display(int n);
