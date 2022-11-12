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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Typedefs.h"
#include "iface_globals.h"
#include "is_fileio.h"

#include "textstr.h"

char *textbuffer;	
char *textstring[STR_MAX];

void textstrings_init()
{
    IS_FileHdl ini;
	char s1[64], s2[1024];
	char end;
	int num;
	int flag;
	char *str;

	ini = IS_Open_Read("gamedata/strings.ini");
	if (!ini)
		return;

	textbuffer = (char *)calloc(STRINGBUFFER_SIZE,sizeof(char));

	str = textbuffer;
	end = 0; num = 0; flag = 0;
	while (!end)
	{
		end = read_line(ini, s1, s2);
		if (!strcmp(s1, "STRINGS"))
			flag=1;
		else if (!strcmp(s1, "END"))
			flag = 0;
		else if (flag)
		{
			strcpy(str, s2);
			textstring[num]=str;
			str+=strlen(s2)+1;
			num++;
		}
	}
	IS_Close(ini);

}

void textstrings_deinit()
{
	free(textbuffer);
}
