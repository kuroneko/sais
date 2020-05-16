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
#include "interface.h"
#include "starmap.h"
#include "combat.h"

#include "cards.h"

// ----------------
//     CONSTANTS
// ----------------

char *ecard_keywords[eckMax] =
{
	"CARD",
	"NAME",
	"TEXT",
	"TEX2",
	"TYPE",
	"PARM",
	"END",
};

// ----------------
// GLOBAL VARIABLES
// ----------------

t_eventcard			*ecards;
int32						num_ecards;

// ----------------
// LOCAL PROTOTYPES
// ----------------

// ----------------
// GLOBAL FUNCTIONS
// ----------------

void cards_init()
{
	FILE* ini;
	char s1[64], s2[256];
	char end;
	int num;
	int flag;
	int n, com;
	int numtypes = 0;
	char cardtypenames[16][32];

	ini = myopen("gamedata/cards.ini", "rb");
	if (!ini)
		return;
 
	end = 0; num = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		if (!strcmp(s1, ecard_keywords[eckBegin]))
			num++;
		else if (!strcmp(s1, "CARDTYPES"))
		{
			flag = 1; n = 0;
		}
		else if (flag == 1)
		{
			if (!strcmp(s1, "END"))
			{	numtypes = n; flag = 0; }
			else
				strcpy(cardtypenames[n++], s1);
		}
	}
	fclose(ini);

	ecards = (t_eventcard*)calloc(num, sizeof(t_eventcard));
	if (!ecards)
		return;
	num_ecards = num;

	ini = myopen("gamedata/cards.ini", "rb");

	end = 0; num = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		com = -1;
		for (n = 0; n < eckMax; n++)
			if (!strcmp(s1, ecard_keywords[n]))
				com = n;

		if (flag == 0)
		{
			if (com == eckBegin)
			{
				flag = 1;
				strcpy(ecards[num].text, "\0");
				ecards[num].parm = 0;
			}
		}
		else switch(com)
		{
			case eckName:
			strcpy(ecards[num].name, s2);
			break;

			case eckText:
			strcpy(ecards[num].text, s2);
			ecards[num].text[strlen(s2)]=0;
			break;

			case eckText2:
			strcpy(ecards[num].text2, s2);
			ecards[num].text2[strlen(s2)]=0;
			break;

			case eckType:
			for (n = 0; n < numtypes; n++)
				if (!strcmp(s2, cardtypenames[n]))
					ecards[num].type = n;
			break;

			case eckParam:
			if (ecards[num].type == card_ally)
			{
				ecards[num].parm = 0;
				for (n = 0; n < num_shiptypes; n++)
					if (!strcmp(s2, shiptypes[n].name))
						ecards[num].parm = n;
			}
			else if (ecards[num].type == card_event)
			{
				sscanf(s2, "%d", &n);
				ecards[num].parm = n;
			}
			break;

			case eckEnd:
			if ((ecards[num].type == card_item) || (ecards[num].type == card_rareitem) || (ecards[num].type == card_lifeform))
			{
				for (n = 0; n < num_itemtypes; n++)
					if (!strcmp(ecards[num].name, itemtypes[n].name))
						ecards[num].parm = n;
			}

			num++; flag = 0;
			break;

			default: ;
		}

	}
	fclose(ini);
}

void cards_deinit()
{
	num_ecards = 0;
	free(ecards);
}

void card_display(int n)
{
	int32 mc, c;
	int32 end = 0;

	prep_screen();
	interface_drawborder(screen,
											 224, 112, 416, 368,
											 1, STARMAP_INTERFACE_COLOR, ecards[n].name);
	interface_textbox(screen, font_6x8,
										240, 136, 160, 224, 0,
										ecards[n].text);
	ik_blit();

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	
		c = ik_inkey();

		if (c == 13)
			end = 1;
	}

}