// ----------------
//     INCLUDES
// ----------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "typedefs.h"
#include "gfx.h"
#include "is_fileio.h"
#include "combat.h"
#include "starmap.h"

FILE *logfile;
int last_logdate;

char moddir[256];

// ----------------
// GLOBAL FUNCTIONS
// ----------------

FILE *myopen(const char *fname, const char *flags)
{
	FILE *fil;
	char realfname[256];

	sprintf(realfname, "%s%s", moddir, fname);
	fil = fopen(realfname, flags);

	if (!fil)
		fil = fopen(fname, flags);

	return fil;
}

int read_line(FILE *in, char *out1, char *out2)
{
	char c;
	char *sptr;
	int flag;
	int end;

	// flag
	// 0: start of line
	// 1: get command symbol
	// 2: between command and params
	// 3: get params

	out1[0] = out2[0] = 0;

	end = 0;
	flag = 0; sptr = out1;
	while (!end)
	{
		c = fgetc(in);
		if ((c < 32 && flag > 0) || feof(in))  // end of line
		{
			*sptr = 0;
			if (!feof(in))
				end = 1; 
			else
				end = 2;
		}
		else
		{
			if (flag == 0 && c > 32)
				flag = 1;
			if (flag == 1 && c <= 32)
				flag = 2;
			if (flag == 2 && c > 32)
			{ flag = 3; *sptr = 0; sptr = out2; }

			if (flag & 1)
				*sptr++ = c;
		}
	}

	//		printf("%s - %s\n", out1, out2);

	return end-1;
}

int read_line1(FILE *in, char *out1)
{
	char c;
	char *sptr;
	int flag;
	int end;

	// flag
	// 0: start of line
	// 1: get command symbol
	// 2: between command and params
	// 3: get params

	out1[0] = 0;

	end = 0;
	flag = 0; sptr = out1;
	while (!end)
	{
		c = fgetc(in);
		if ((c < 32 && flag > 0) || feof(in))  // end of line
		{
			*sptr = 0;
			if (!feof(in))
				end = 1; 
			else
				end = 2;
		}
		else
		{
			if (flag == 0 && c > 32)
				flag = 1;
			if (flag & 1)
				*sptr++ = c;
		}
	}

	//		printf("%s - %s\n", out1, out2);

	return end-1;
}

void ik_start_log()
{
	int n;
	FILE *fil;
	char fname[32];
	
	logfile = NULL;

	n=0;
	while (n<1000)
	{
		sprintf(fname, "log%04d.txt", n);
		fil = myopen(fname, "rt");
		if (!fil)
		{
			logfile = myopen(fname, "wt");
//			ik_save_tga(fname, img, pal);
			break;
		}
		else fclose(fil);

		n++;
	}
	last_logdate = -1;
}

void ik_print_log(char *ln, ...)
{
	char dlin[256], *dl=dlin;
	char text1[256], text2[256];
	int d, m, y;
	int date = player.stardate;

	if (!logfile)
		return;

  va_list ap;
  va_start(ap, ln);
  vsprintf(dlin, ln, ap);
  va_end(ap);

	if (date > last_logdate)
	{
		d = date%365;
		for (y = 0; y < 12; y++)
			if (d >= months[y].sd && d < months[y].sd+months[y].le)
				m = y;
		d = d + 1 - months[m].sd;
		y = 4580 + (date/365);
		sprintf(text1, "Captain %s of the %s", player.captname, player.shipname);
		sprintf(text2, "\n%s%%%ds %%02d %%s %%d\n", text1, 52-strlen(text1));
		fprintf(logfile, text2, "Date:", d, months[m].name, y);
		fprintf(logfile, "================================================================\n");
		last_logdate = date;
	}

	fprintf(logfile, dl);
}