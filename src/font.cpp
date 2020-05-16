#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <malloc.h>

#include "typedefs.h"
#include "iface_globals.h"
#include "gfx.h"
#include "snd.h"

void ik_drawfont(t_ik_image *img, t_ik_font *fnt, int32 x, int32 y, uint8 co, uint8 c);

// LOAD FONT FROM PCX
t_ik_font *ik_load_font(char *fname, uint8 w,  uint8 h)
{
	t_ik_font *fnt;
	t_ik_image *pic;
	int32 x, y, x2, y2, tmp;

	pic=ik_load_pcx(fname, NULL);
	if (!pic)
		return NULL;

	fnt=(t_ik_font *)malloc(sizeof(t_ik_font));
	if (!fnt)
	{
		del_image(pic);
		return NULL;
	}

	fnt->w=w;
	fnt->h=h; 
	fnt->data=(uint8 *)malloc(128*w*h);
	if (!fnt->data)
	{
		free(fnt);
		del_image(pic);
		return NULL;
	}

	for (y=0; y<4; y++)
		for (x=0; x<32; x++)
		{
			tmp=y*32+x;
			for (y2=0;y2<h;y2++)
				for (x2=0;x2<w;x2++)
				{
					fnt->data[tmp*w*h+y2*w+x2]=pic->data[(y*h+y2)*pic->w+(x*w+x2)];
				}
		}

	del_image(pic);
	return fnt;
}

void ik_del_font(t_ik_font *fnt)
{
	if (!fnt)
		return;
	if (fnt->data)
		free(fnt->data);
	free(fnt);
}

// DRAW SINGLE LETTER 
void ik_drawfont(t_ik_image *img, t_ik_font *fnt, int32 x, int32 y, uint8 co, uint8 c)
{
	int32 x1, y1, o;
  uint8 v;

	if (!fnt) return;
  if (x<-fnt->w || y<-fnt->h || x>gfx_width || y>gfx_height || c>127)  return;

	o=fnt->w*fnt->h;

  for (y1=0; y1<fnt->h; y1++)
  {
  	if (y1+y>=c_miny && y1+y<c_maxy)
	  	for (x1=0; x1<fnt->w; x1++)
  	  {
      	if (x1+x>=c_minx && x1+x<c_maxx)
        {
        	v=fnt->data[c*o+y1*fnt->w+x1];
          if (v)
						ik_putpixel(img, x1+x,y1+y,v+(co<<4));
        }
    	}
  }
}

void ik_drawfontbig(t_ik_image *img, t_ik_font *fnt, int32 x, int32 y, uint8 co, uint8 c)
{
	int32 x1, y1, o;
  uint8 v;

	if (!fnt) return;
  if (x<-fnt->w<<1 || y<-fnt->h<<1 || x>gfx_width || y>gfx_height || c>127)  return;

	o=fnt->w*fnt->h;

  for (y1=0; y1<fnt->h<<1; y1++)
  {
  	if (y1+y>=c_miny && y1+y<c_maxy)
	  	for (x1=0; x1<fnt->w<<1; x1++)
  	  {
      	if (x1+x>=c_minx && x1+x<c_maxx)
        {
        	v=fnt->data[c*o+(y1>>1)*fnt->w+(x1>>1)];
          if (v)
						ik_putpixel(img, x1+x,y1+y,v+(co<<4));
        }
    	}
  }
}

// PRINT A LINE
void ik_print(t_ik_image *img, t_ik_font *fnt, int32 x, int32 y, uint8 co, char *ln, ...)
{
	char dlin[256], *dl=dlin;
  va_list ap;
  va_start(ap, ln);
  vsprintf(dlin, ln, ap);
  va_end(ap);

	if (!fnt)
		return;
	if (!fnt->data)
		return;

  while (*dl)
	{
  	ik_drawfont(img, fnt, x, y, co, (*dl++));
		x+=fnt->w;
  }
}

void ik_printbig(t_ik_image *img, t_ik_font *fnt, int32 x, int32 y, uint8 co, char *ln, ...)
{
	char dlin[256], *dl=dlin;
  va_list ap;
  va_start(ap, ln);
  vsprintf(dlin, ln, ap);
  va_end(ap);

	if (!fnt)
		return;
	if (!fnt->data)
		return;

  while (*dl)
	{
  	ik_drawfontbig(img, fnt, x, y, co, (*dl++));
		x+=fnt->w<<1;
  }
}

void ik_text_input(int x, int y, int l, t_ik_font *fnt, char *pmt, char *tx, int bg, int co)
{
	int end=0,n,upd=1;
	char c;
	int t, ot;
	int l2 = strlen(pmt);
	uint32 p;
	t_ik_sprite *bup;

	n=strlen(tx);
	if (n>=l-l2)
	{
		n=l-1-l2;
		tx[l]=0;
	}

	prep_screen();
	bup=get_sprite(screen, x, y, fnt->w*l, fnt->h);
	free_screen();

	start_ik_timer(3, 500); 
	t=0;ot=0;

	while (!end)
	{
		ik_eventhandler();  // always call every frame
		c=ik_inkey();
		ot=t; t=get_ik_timer(2);

		if (ot!=t)
			upd=1;

		if (c>=32 && c<128 && n<l-1-l2)
		{
			tx[n]=c;
			n++;
			tx[n]=0;
			upd=1;
		}

		if (c==8 && n>0)
		{
			n--;
			tx[n]=0;
			upd=1;
		}

		if (c==27 || must_quit!=0) end=1;
		if (c==13) end=2;

		if (upd>0 && end==0)
		{
			prep_screen();
			ik_drawbox(screen, x, y, x+fnt->w*l-1, y+fnt->h-1, bg);
//			ik_dsprite(screen, x, y, bup, 0);
			if (get_ik_timer(3)&1)
				ik_print(screen,fnt,x,y,co,"%s%s",pmt,tx);
			else
				ik_print(screen,fnt,x,y,co,"%s%s_",pmt,tx);
			ik_blit();
			upd=0;
		}
	}

	prep_screen();
	ik_drawbox(screen, x, y, x+fnt->w*l-1, y+fnt->h-1, 0);
	ik_dsprite(screen, x, y, bup, 0);
	free_sprite(bup);
	ik_blit();

	if (end==1)
		tx[0]=0;
	else	// capitalize first letter of each word
	{
		for (p = 0; p < strlen(tx); p++)
		{
			if (p==0 || (p>0 && (tx[p-1]<=' ' || tx[p-1]=='-')))
			{
				if (tx[p] >= 'a' && tx[p] <= 'z')
					tx[p] += 'A' - 'a';
			}
		}
	}
}

