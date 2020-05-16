#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "typedefs.h"
#include "iface_globals.h"
#include "is_fileio.h"

#include "textstr.h"

char *textbuffer;	
char *textstring[STR_MAX];

void textstrings_init()
{
	FILE* ini;
	char s1[64], s2[1024];
	char end;
	int num;
	int flag;
	char *str;

	ini = myopen("gamedata/strings.ini", "rb");
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
	fclose(ini);

}

void textstrings_deinit()
{
	free(textbuffer);
}