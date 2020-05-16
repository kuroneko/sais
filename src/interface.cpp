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
#include "textstr.h"

#include "interface.h"

// ----------------
//     CONSTANTS
// ----------------

// ----------------
// GLOBAL VARIABLES
// ----------------

t_ik_spritepak *spr_IFborder;
t_ik_spritepak *spr_IFbutton;
t_ik_spritepak *spr_IFslider;
t_ik_spritepak *spr_IFarrows;
t_ik_spritepak *spr_IFsystem;
t_ik_spritepak *spr_IFtarget;
t_ik_spritepak *spr_IFdifnebula;
t_ik_spritepak *spr_IFdifenemy;


t_ik_font			 *font_4x8;
t_ik_font			 *font_6x8;

int tut_seen[tut_max];

// ----------------
// LOCAL PROTOTYPES
// ----------------

void interface_initsprites();
void interface_deinitsprites();

// ----------------
// GLOBAL FUNCTIONS
// ----------------

void interface_init()
{
	interface_initsprites();
}

void interface_deinit()
{
	interface_deinitsprites();
}

void interface_drawborder(t_ik_image *img, 
													int32 left, int32 top, int32 right, int32 bottom,
													int32 fill, int32 color,
													char *title)
{
	int32 x, y;
	int32 flag;
	char title2[128];

	strcpy(title2, title);
	y = strlen(title2);
	for (x = 0; x < y; x++)
	{	
		if (title2[x] >= 'a' && title2[x] <= 'z')
			title2[x] -= ('a' - 'A');
	}

	if (!color)
		flag = 0;
	else
		flag = 2 + (color << 8);

	if (fill) fill=9;

	for (y = top + 24; y < bottom - 16; y += 16)
	{
		if (fill)
			for (x = left + 32; x < right - 32; x += 32)
				ik_dsprite(img, x, y, spr_IFborder->spr[13], flag);

		ik_dsprite(img, left, y, spr_IFborder->spr[3 + fill], flag);
		ik_dsprite(img, right-32, y, spr_IFborder->spr[5 + fill], flag);
	}
	for (x = left+32; x < right - 32; x += 32)
	{
		ik_dsprite(img, x, top, spr_IFborder->spr[1 + fill], flag);
		ik_dsprite(img, x, bottom-16, spr_IFborder->spr[7 + fill], flag);
	}
	ik_dsprite(img, left, top,								spr_IFborder->spr[fill], flag);
	ik_dsprite(img, right - 32, top,					spr_IFborder->spr[2 + fill], flag);
	ik_dsprite(img, left, bottom - 16,				spr_IFborder->spr[6 + fill], flag);
	ik_dsprite(img, right - 32, bottom - 16,	spr_IFborder->spr[8 + fill], flag);

	if (title)
	{
		ik_print(img, font_6x8, left+16, top+6, color, title2);
	}
}

void interface_thinborder(t_ik_image *img, 
													int32 left, int32 top, int32 right, int32 bottom,
													int32 color, int32 fill)
{
	int32 x, y;
	int32 flag;

	if (!color)
		flag = 0;
	else
		flag = 2 + (color << 8);

	if (fill > -1) 
		ik_drawbox(img, left, top, right-1, bottom-1, fill);

	for (y = top + 8; y < bottom - 8; y += 8)
	{
		ik_dsprite(img, left, y, spr_IFborder->spr[IF_BORDER_SMALL + 3], flag);
		ik_dsprite(img, right-8, y, spr_IFborder->spr[IF_BORDER_SMALL + 5], flag);
	}
	for (x = left+8; x < right - 8; x += 8)
	{
		ik_dsprite(img, x, top, spr_IFborder->spr[IF_BORDER_SMALL + 1], flag);
		ik_dsprite(img, x, bottom-8, spr_IFborder->spr[IF_BORDER_SMALL + 7], flag);
	}
	ik_dsprite(img, left, top,							spr_IFborder->spr[IF_BORDER_SMALL], flag);
	ik_dsprite(img, right - 8, top,					spr_IFborder->spr[IF_BORDER_SMALL + 2], flag);
	ik_dsprite(img, left, bottom - 8,				spr_IFborder->spr[IF_BORDER_SMALL + 6], flag);
	ik_dsprite(img, right - 8, bottom - 8,	spr_IFborder->spr[IF_BORDER_SMALL + 8], flag);
}

void interface_drawslider(t_ik_image *img, int32 left, int32 top, int32 a, int32 l, int32 rng, int32 val, int32 color)
{
	int32 x,y ;

	if (a)	// vertical
	{
		for (y = top + 8; y < top + l - 8; y+=8)
			ik_dsprite(img, left, y, spr_IFslider->spr[5], 2+(color<<8));
		ik_dsprite(img, left, top, spr_IFslider->spr[4], 2+(color<<8));
		ik_dsprite(img, left, top + l - 8, spr_IFslider->spr[6], 2+(color<<8));
		ik_dsprite(img, left, (top * (rng-val) + (top + l - 8) * val) / rng, spr_IFslider->spr[7], 2+(color<<8));
	}
	else		// horizontal
	{
		for (x = left + 8; x < left + l - 8; x+=8)
			ik_dsprite(img, x, top, spr_IFslider->spr[1], 2+(color<<8));
		ik_dsprite(img, left, top, spr_IFslider->spr[0], 2+(color<<8));
		ik_dsprite(img, left + l - 8, top, spr_IFslider->spr[2], 2+(color<<8));
		ik_dsprite(img, (left * (rng-val) + (left + l - 8) * val) / rng, top, spr_IFslider->spr[3], 2+(color<<8));
	}

}

void interface_drawbutton(t_ik_image *img, int32 left, int32 top, int32 l, int32 color, char *text)
{
	int32 x;

	for (x = left + 16; x < left + l - 16; x+=16)
		ik_dsprite(img, x, top, spr_IFbutton->spr[5], 2+(color<<8));
	ik_dsprite(img, left, top, spr_IFbutton->spr[4], 2+(color<<8));
	ik_dsprite(img, left + l - 16, top, spr_IFbutton->spr[6], 2+(color<<8));
	ik_print(img, font_6x8, left + l/2 - strlen(text)*3, top + 5, color, text);
}

int32 interface_textbox(t_ik_image *img, t_ik_font *fnt,
											 int32 left, int32 top, int32 w, int32 h,
											 int32 color, 
											 char *text)
{
	char *txb;
	char *txp;
	char *spc;
	int32 x, y;
	int32 c, l;
	char lne[80];

	txb = text; 
	y = top;
	l = strlen(text);
	c = 0;
	while ( y <= top + h - fnt->h && c < l)
	{
		spc = txp = txb; x = 0;
		while ( x <= w && c < l)
		{
			if (*txp <= ' ' || *txp == '|')
			{
				spc = txp;
			}
			if (*txp == 0)
			{	c = l; break; }
			else
			{
				if (*txp == '|')
					x = w;
				x += fnt->w;
				txp++;
				c++;
			}
		}
		if (c >= l)
		{
			strncpy(lne, txb, txp-txb);
			lne[txp-txb]=0;
		}
		else if (spc > txb)
		{
			strncpy(lne, txb, spc-txb);
			lne[spc-txb]=0;
			c-=(spc-txb)-1;
			txb = spc+1;
		}
		else
		{
			strncpy(lne, txb, txp-txb);
			lne[txp-txb]=0;
			txb = txp;
		}
		for (x = 0; x < (int)strlen(lne); x++)
			if (lne[x] == '|')  
				lne[x] = ' ';
		ik_print(img, fnt, left, y, color, lne);
//		ik_print(img, fnt, left-8, y, color, "%d", (y-top)/fnt->h);

		y += fnt->h;
	}
//	ik_print(img, fnt, left, top, 2, "LEN: %d %d", l, c);
	return (y - top)/fnt->h;
}

int32 interface_textboxsize(t_ik_font *fnt,
														int32 w, int32 h,
														char *text)
{
	char *txb;
	char *txp;
	char *spc;
	int32 x, y;
	int32 c, l;
	char lne[80];

	txb = text; 
	y = 0;
	l = strlen(text);
	c = 0;
	while ( y*fnt->h <= h - fnt->h && c < l)
	{
		spc = txp = txb; x = 0;
		while ( x <= w && c < l)
		{
			if (*txp <= ' ' || *txp == '|')
			{
				spc = txp;
			}
			if (*txp == 0)
			{	c = l; break; }
			else
			{
				if (*txp == '|')
					x = w;
				x += fnt->w;
				txp++;
				c++;
			}
		}
		if (c >= l)
		{
			strncpy(lne, txb, txp-txb);
			lne[txp-txb]=0;
		}
		else if (spc > txb)
		{
			strncpy(lne, txb, spc-txb);
			lne[spc-txb]=0;
			c-=(spc-txb)-1;
			txb = spc+1;
		}
		else
		{
			strncpy(lne, txb, txp-txb);
			lne[txp-txb]=0;
			txb = txp;
		}

		y++;
	}
	return y;
}

int32 interface_popup(t_ik_font *fnt, 
										 int32 left, int32 top, int32 w, int32 h,
										 int32 co1, int32 co2, 
										 char *label, char *text, char *button1, char *button2, char *button3)
{
	int32 mc, c;
	int32 end = 0;
	int32 bl = 0;
	int32 t;
	t_ik_sprite *bg;
	int bc[3];

	bc[0] = bc[1] = bc[2] = 0;

	if (button1)
	{	bl = MAX((int32)strlen(button1), bl); bc[0] = button1[0]; }
	if (button2)
	{	bl = MAX((int32)strlen(button2), bl); bc[1] = button2[0]; }
	if (button3)
	{	bl = MAX((int32)strlen(button3), bl); bc[2] = button3[0]; }

	
	for (c = 0; c < 3; c++)
	{
		if (bc[c] >= 'a' && bc[c] <='z')
			bc[c] += 'A'-'a';
	}

	if (bl)
	{
		bl = bl * 6 + 16;
		bl = bl & 0xf8;
		if (bl<32) bl=32;
	}

	if (!h)
	{
		h = interface_textboxsize(fnt, w - 32, 256, text)*8 + 32 + 16*(bl>0);
	}

	if (left == -1)
	{
		left = screen->w/2 - w/2;
	}

	if (top == -1)
	{
		top = (screen->h * 2)/5 - h/2;
	}

	prep_screen();

	bg = get_sprite(screen, left, top, w, h);

	interface_drawborder(screen,
											 left, top, left+w, top+h,
											 1, co1, label);

	interface_textbox(screen, fnt,
										left+16, top+24, w - 32, h - 32 - (bl>0)*16, co2,
										text);

	if (button1)
		interface_drawbutton(screen, left+w-16-bl, top + h - 24, bl, co1, button1);
	if (button2)
		interface_drawbutton(screen, left+16, top + h - 24, bl, co1, button2);
	if (button3)
		interface_drawbutton(screen, left+w/2-bl/2, top + h - 24, bl, co1, button3);

	ik_blit();

	while (!must_quit && !end)
	{
		ik_eventhandler();  // always call every frame
		mc = ik_mclick();	
		c = ik_inkey();

		if (c >= 'a' && c <= 'z')
			c += 'A' - 'a';

		if (!button1)
		{
			if (c == 13 || mc == 1)
				end = 1;
		}
		else
		{
			if (c == bc[0])
				end = 1;

			if (!button2)
			{
				if (c == 13)
					end = 1;
			}
			else
				if (c == bc[1])
					end = 2;

			if (mc == 1 && ik_mouse_y > top+h-24 && ik_mouse_y < top+h-8)
			{
				if (ik_mouse_x > left+w-16-bl && ik_mouse_x < left+w-16)
					end = 1;
				if (button2)
				{
					if (ik_mouse_x > left+16 && ik_mouse_x < left+16+bl)
						end = 2;
				}
			}
		}	

		c = t; t = get_ik_timer(2);
		if (t != c)
		{ prep_screen(); ik_blit(); }
	}

	if (button1 && button2)	// yes/no
	{
		if (end == 1)
			Play_SoundFX(WAV_YES, get_ik_timer(0));
		else if (end == 2)
			Play_SoundFX(WAV_NO, get_ik_timer(0));
	}
	else
		Play_SoundFX(WAV_DOT, get_ik_timer(0));

	prep_screen();
	ik_dsprite(screen, left, top, bg, 4);
	ik_blit();
	free_sprite(bg);

	if (must_quit)
	{
		must_quit = 0;
		end = 2;
	}

	return end-1;
}

// ----------------
// LOCAL FUNCTIONS
// ----------------

void interface_initsprites()
{
	t_ik_image *pcx;	
	int x, y, n;

	font_4x8 = ik_load_font("graphics/fnt2.pcx", 4, 8);
	font_6x8 = ik_load_font("graphics/fnt3.pcx", 6, 8);

	spr_IFborder = load_sprites("graphics/ifborder.spr");
	spr_IFbutton = load_sprites("graphics/ifbutton.spr");
	spr_IFslider = load_sprites("graphics/ifslider.spr");
	spr_IFarrows = load_sprites("graphics/ifarrows.spr");
	spr_IFsystem = load_sprites("graphics/ifsystem.spr");
	spr_IFtarget = load_sprites("graphics/iftarget.spr");
	spr_IFdifnebula = load_sprites("graphics/ifdifneb.spr");
	spr_IFdifenemy = load_sprites("graphics/ifdifnmy.spr");

	pcx = NULL;

	if (!spr_IFborder)
	{
		pcx = ik_load_pcx("interfce.pcx", NULL);
		spr_IFborder = new_spritepak(30);
		for (n=0;n<3;n++)
		{
			spr_IFborder->spr[n] = get_sprite(pcx, n*32, 0, 32, 24);
		}
		for (n=0;n<6;n++)
		{
			x = n%3; y = n/3;
			spr_IFborder->spr[n+3] = get_sprite(pcx, x*32, y*16+24, 32, 16);
		}
		for (n=0;n<3;n++)
		{
			spr_IFborder->spr[n+9] = get_sprite(pcx, n*32, 56, 32, 24);
		}
		for (n=0;n<6;n++)
		{
			x = n%3; y = n/3;
			spr_IFborder->spr[n+12] = get_sprite(pcx, x*32, y*16+80, 32, 16);
		}

		spr_IFborder->spr[18] = get_sprite(pcx, 96, 0, 64, 64);
		spr_IFborder->spr[19] = get_sprite(pcx, 192, 0,128,128);
		spr_IFborder->spr[20] = get_sprite(pcx, 96, 64,64,32);

		for (n=0; n<9; n++)
		{
			spr_IFborder->spr[n+21] = get_sprite(pcx, 160, n*8, 8, 8);
		}

		save_sprites("graphics/ifborder.spr", spr_IFborder);
	}

	if (!spr_IFdifnebula)
	{
		if (!pcx)
			pcx = ik_load_pcx("interfce.pcx", NULL);
		spr_IFdifnebula = new_spritepak(3);
		for (n=0;n<3;n++)
		{
			spr_IFdifnebula->spr[n] = get_sprite(pcx, n*64, 192, 64, 64);
		}

		save_sprites("graphics/ifdifneb.spr", spr_IFdifnebula);
	}

	if (!spr_IFdifenemy)
	{
		if (!pcx)
			pcx = ik_load_pcx("interfce.pcx", NULL);
		spr_IFdifenemy = new_spritepak(6);
		for (n=0;n<3;n++)
		{
			spr_IFdifenemy->spr[n] = get_sprite(pcx, (n&1)*64+192, 192+(n/2)*32, 64, 32);
			spr_IFdifenemy->spr[n+3] = get_sprite(pcx, n*64, 256, 64, 32);
		}

		save_sprites("graphics/ifdifnmy.spr", spr_IFdifenemy);
	}

	if (!spr_IFbutton)
	{
		if (!pcx)
			pcx = ik_load_pcx("interfce.pcx", NULL);
		spr_IFbutton = new_spritepak(22);
		for (n=0;n<7;n++)
		{
			spr_IFbutton->spr[n] = get_sprite(pcx, n*16, 128, 16, 16);
		}
		for (n=0;n<4;n++)
		{
			spr_IFbutton->spr[n+7] = get_sprite(pcx, n*8+112, 128, 8, 16);
			spr_IFbutton->spr[n+11] = get_sprite(pcx, n*32+176, 128, 32, 16);
		}
		spr_IFbutton->spr[15] = get_sprite(pcx, 304, 128, 16, 8);
		spr_IFbutton->spr[16] = get_sprite(pcx, 0, 176, 32, 16);
		spr_IFbutton->spr[17] = get_sprite(pcx, 144, 128, 16, 16);
		spr_IFbutton->spr[18] = get_sprite(pcx, 160, 128, 16, 16);
		spr_IFbutton->spr[19] = get_sprite(pcx, 168, 0, 24, 64);
		spr_IFbutton->spr[20] = get_sprite(pcx, 256, 224, 32, 16);
		spr_IFbutton->spr[21] = get_sprite(pcx, 288, 224, 32, 16);

		save_sprites("graphics/ifbutton.spr", spr_IFbutton);
	}

	if (!spr_IFslider)
	{
		if (!pcx)
			pcx = ik_load_pcx("interfce.pcx", NULL);
		spr_IFslider = new_spritepak(10);
		for (n=0;n<10;n++)
		{
			spr_IFslider->spr[n] = get_sprite(pcx, n*8, 112, 8, 8);
		}

		save_sprites("graphics/ifslider.spr", spr_IFslider);
	}

	if (!spr_IFarrows)
	{
		if (!pcx)
			pcx = ik_load_pcx("interfce.pcx", NULL);
		spr_IFarrows = new_spritepak(16);
		for (n=0;n<16;n++)
		{
			spr_IFarrows->spr[n] = get_sprite(pcx, n*8, 120, 8, 8);
		}

		save_sprites("graphics/ifarrows.spr", spr_IFarrows);
	}

	if (!spr_IFsystem)
	{
		if (!pcx)
			pcx = ik_load_pcx("interfce.pcx", NULL);
		spr_IFsystem = new_spritepak(17);
		for (n=0;n<17;n++)
		{
			spr_IFsystem->spr[n] = get_sprite(pcx, n*16, 144, 16, 16);
		}

		save_sprites("graphics/ifsystem.spr", spr_IFsystem);
	}

	if (!spr_IFtarget)
	{
		if (!pcx)
			pcx = ik_load_pcx("interfce.pcx", NULL);
		spr_IFtarget = new_spritepak(10);
		for (n=0;n<9;n++)
		{
			spr_IFtarget->spr[n] = get_sprite(pcx, n*16, 160, 16, 16);
		}
		spr_IFtarget->spr[9] = get_sprite(pcx, 144, 160, 32, 32);

		save_sprites("graphics/iftarget.spr", spr_IFtarget);
	}

	if (pcx)
		del_image(pcx);
}

void interface_deinitsprites()
{
	free_spritepak(spr_IFborder);
	free_spritepak(spr_IFbutton);
	free_spritepak(spr_IFslider);
	free_spritepak(spr_IFarrows);
	free_spritepak(spr_IFsystem);
	free_spritepak(spr_IFtarget);
	free_spritepak(spr_IFdifnebula);
	free_spritepak(spr_IFdifenemy);

	ik_del_font(font_6x8);
	ik_del_font(font_4x8);
}

void interface_cleartuts()
{
	int i;

	for (i = 0; i < tut_max; i++)
		tut_seen[i] = 0;
}

void interface_tutorial(int n)
{
	int r;

	if (tut_seen[n])
		return;

	r = interface_popup(font_6x8, -1, -1, 288, 0, 12, 0, 
			textstring[STR_TUT_TSTARMAP + n],
			textstring[STR_TUT_STARMAP + n], 
			textstring[STR_OK], textstring[STR_TUT_END]);

	if (r)
		settings.random_names -= (settings.random_names & 4);
	/*
	switch(n)
	{
		case tut_starmap:
			interface_popup(font_6x8, -1, -1, 288, 0, 11, 0, 
				"Tutorial: starmap",
				"Your starship is located near the bottom of the screen, in the Glory star system. Left click any other star, and a green STAR LANE will appear. Try to make sure that the star lane does not pass through obstacles like a nebula or black hole. When you are ready to travel to the selected star, left click on the word ENGAGE. You can also press F1 at any time to view a comprehensive HELP SCREEN.", 
				"OK");
		break;

		case tut_explore:
			interface_popup(font_6x8, -1, -1, 288, 0, 11, 0, 
				"Tutorial: planetary exploration",
				"This window displays the most interesting planet in orbit around this star. It also lists information about the planet environment and more importantly the DISCOVERY you have made there. The discovery will be either an EVENT or an ITEM. You may also rename the planet in this window by clicking on its randomized name.", 
				"OK");
		break;

		case tut_upgrade:
			interface_popup(font_6x8, -1, -1, 288, 0, 11, 0, 
				"Tutorial: ship upgrade",
				"You have discovered an item which may be used to UPGRADE your ship. To INSTALL it, left click the yellow triangle icon next its name in the CARGO window. You may also UNINSTALL a ship component by clicking the yellow 'X' symbol next to an already installed system under the ship silhouette.", 
				"OK");
		break;

		case tut_device:
			interface_popup(font_6x8, -1, -1, 288, 0, 11, 0, 
				"Tutorial: alien device",
				"You have discovered a DEVICE of alien or unknown origin. Look at the CARGO window and left click the rectangular yellow icon next to the name of the item to USE it.", 
				"OK");
		break;

		case tut_treasure:
			interface_popup(font_6x8, -1, -1, 288, 0, 11, 0, 
				"Tutorial: treasure",
				"You have discovered a VALUABLE ITEM or an UNUSUAL LIFEFORM which has been added to your CARGO window. In the end of the game, you will sell it to Lextor Mucron for a profit.", 
				"OK");
		break;

		case tut_ally:
			interface_popup(font_6x8, -1, -1, 288, 0, 11, 0, 
				"Tutorial: allied ship",
				"A new ALLY has been added to your flotilla. Click the small ship icons in the upper left hand corner of the screen to view other starships besides your own. The icon will blink if the ship is damaged.", 
				"OK");
		break;

		case tut_encounter:
			interface_popup(font_6x8, -1, -1, 288, 0, 11, 0, 
				"Tutorial: alien encounter",
				"You have encountered one of the many alien races which inhabit the Purple Void. The RADAR SCREEN displays how many alien starships are in orbit around this star, and their relative sizes. You must choose to encounter the alien patrol or flee. If you choose to encounter the aliens, left click ENGAGE. If you wish to flee, left click AVOID and you will return to a previously explored star.", 
				"OK");
		break;

		case tut_combat:
			interface_popup(font_6x8, -1, -1, 288, 0, 11, 0, 
				"Tutorial: space combat",
				"You have engaged one or more enemy starships. Left click on a friendly ship to select it, then left click anywhere on the tactical map including enemy ships. Your ship will then begin to move towards that point at best speed. When a hostile ship is within range of your weapons, they will fire automatically. You can also press F1 at any time to view a comprehensive HELP SCREEN.", 
				"OK");
		break;

		default: return;
	}
	*/
	tut_seen[n] = 1;
}