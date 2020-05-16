// ----------------
//     INCLUDES
// ----------------

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <io.h>

#include "typedefs.h"
#include "iface_globals.h"
#include "gfx.h"
#include "is_fileio.h"
#include "snd.h"

//		FILE *loggy;

// --------------------------------------------
//      SPRITE CREATION AND MANAGEMENT
// --------------------------------------------

// CREATE NEW SPRITE TEMPLATE
t_ik_sprite *new_sprite(int32 w, int32 h)
{
	t_ik_sprite *spr;

	spr=(t_ik_sprite *)malloc(sizeof(t_ik_sprite));
	if (!spr)
		return NULL;

	spr->data=(uint8 *)malloc(w*h);
	spr->w=w;
	spr->h=h;
	spr->co=0;

	return spr;
}

// FIND APPROXIMATE COLOR OF SPRITE
int32 calc_sprite_color(t_ik_sprite *spr)
{
	int32 x,y,r,g,b,npx,c;

	if (!spr)
		return 0;

	r=0;g=0;b=0;npx=0;
	for (y=0;y<spr->h;y++)
	for (x=0;x<spr->w;x++)
	{
		c=spr->data[y*spr->w+x];
		if (c)
		{
			c=get_palette_entry(c);
			r+=(c>>16)&255; g+=(c>>8)&255; b+=c&255;
//			r+=pal[c*3]; g+=pal[c*3+1]; b+=pal[c*3+2];
			npx++;
		}
	}

	if (npx>0)
	{
		spr->co=get_rgb_color(r/npx,g/npx,b/npx);
	}

	return spr->co;
}

// GRAB SPRITE FROM IMAGE
t_ik_sprite *get_sprite(t_ik_image *img, int32 x, int32 y, int32 w, int32 h)
{
	t_ik_sprite *spr;
	int32 x1,y1;

	if (!img)
		return NULL;

	spr=new_sprite(w,h);
	if (!spr)
		return NULL;

	for (y1=0;y1<h;y1++)
		for (x1=0;x1<w;x1++)
		if (y1+y >= 0 && y1+y < img->h && x1+x >= 0 && x1+x < img->w)
		{
			spr->data[y1*w+x1]=img->data[(y1+y)*img->w+(x1+x)];
		}
		else
			spr->data[y1*w+x1]=0;

	calc_sprite_color(spr);

	return spr;
}

// DESTROY SPRITE AND FREE MEMORY
void free_sprite(t_ik_sprite *spr)
{
	if (spr)
	{
		if (spr->data)
			free(spr->data);
		free(spr);
	}
}

// --------------------------------------------
//     SPRITEPAK CREATION AND MANAGEMENT
// --------------------------------------------

t_ik_spritepak *new_spritepak(int32 num)
{
	t_ik_spritepak *pak;

	pak = (t_ik_spritepak*)calloc(1, sizeof(t_ik_spritepak));
	if (!pak)
		return NULL;

	pak->num = num;
	pak->spr = (t_ik_sprite**)calloc(num, sizeof(t_ik_sprite*));

	return pak;
}

void free_spritepak(t_ik_spritepak *pak)
{
	int x;

	if (!pak)
		return;

	for (x = 0; x < pak->num; x++)
	{
		free_sprite(pak->spr[x]);
		pak->spr[x]=NULL;
	}
	free(pak->spr);
	free(pak);
}

t_ik_spritepak *load_sprites(char *fname)
{
	// NOTE: load_sprites loads default .SPR, and FRAMES from the mod
	struct _finddata_t find;
	long fhandle;
	int fi;
	int fnum;
	char spritedir[256];
	char framename[256];
	int rep[256];
	int max;
	t_ik_image *img;
	uint8 *buffu;

	FILE *fil;
	t_ik_spritepak *pak;
	int32 x,num;
	int32 w,h,c;

/*
if loading a mod, look at the folder for new frames
and mark them in the replacement array.
*/
	for (x = 0; x < 256; x++)
		rep[x] = 0;
	max = 0;
	if (strlen(moddir))	
	{
		sprintf(spritedir, "%s%s", moddir, fname);
		sprintf(spritedir+strlen(spritedir)-4, "/\0");
		sprintf(framename, "%sframe*.tga", spritedir);

		fhandle = _findfirst(framename, &find);
		if (fhandle != -1)
		{
			fi = fhandle;
			while (fi != -1)
			{
				sscanf(find.name+5, "%03d", &fnum);
				if (fnum < 256)
				{
					rep[fnum] = 1;
					if (fnum+1 > max)
						max = fnum+1;
				}
				fi = _findnext(fhandle, &find);
			}
			_findclose(fhandle);
		}
	}

	fil=fopen(fname,"rb");	// don't use myopen here
	if (!fil)
		return NULL;

	num=fgetc(fil);
	num+=fgetc(fil)*256;

	if (num > max)
		max = num;

	pak = new_spritepak(max);
	if (!pak)
	{ fclose(fil); return NULL; }

	for (x=0;x<max;x++)
	{
		// header
		if (x < num)
		{
			w=fgetc(fil);
			w+=fgetc(fil)*256;
			h=fgetc(fil);
			h+=fgetc(fil)*256;
			c = fgetc(fil);
			fgetc(fil);
			fgetc(fil);
			fgetc(fil);
			buffu = (uint8*)malloc(w * h);
			fread(buffu,1,w*h,fil);

			if (!rep[x])
			{
			// if not marked as rep, make new sprite
				pak->spr[x]=new_sprite(w,h);
				pak->spr[x]->co=c;
				// data
				memcpy(pak->spr[x]->data, buffu, w*h);
			}

			free(buffu);
		}
		if (rep[x])
		{
			sprintf(framename, "%sframe%03d.tga", spritedir, x);
			img = ik_load_tga(framename, NULL);
			if (img)
			{
				pak->spr[x]=get_sprite(img, 0, 0, img->w, img->h);
				del_image(img);
			}
		}
	}

	fclose(fil);

	return pak;
}

void save_sprites(char *fname, t_ik_spritepak *pak)
{
	FILE *fil;
	int32 x;
	int32 num = pak->num;

	fil=myopen(fname,"wb");
	if (!fil)
		return;

	fputc(num&255, fil);
	fputc(num>>8, fil);

	for (x=0;x<num;x++)
	{
		// header
		fputc(pak->spr[x]->w & 255,fil);
		fputc(pak->spr[x]->w >> 8,fil);
		fputc(pak->spr[x]->h & 255,fil);
		fputc(pak->spr[x]->h >> 8,fil);
		fputc(pak->spr[x]->co,fil);
		// filler
		fputc(0,fil);
		fputc(0,fil);
		fputc(0,fil);
		// data
		fwrite(pak->spr[x]->data,1,pak->spr[x]->w*pak->spr[x]->h,fil);
	}
}

// --------------------------------------------
//	      SPRITE DRAWING FUNCTIONS
// --------------------------------------------

// basic sprite draw.. corner align, 0-masked
// flags:
// 1:  center align (move up-left by half the size)
// 2:  color 
// 4:  blank
void ik_dsprite(t_ik_image *img, int32 x, int32 y, t_ik_sprite *spr, int32 flags)
{
	int32 px,py;
	int32 yb,ye,xb,xe;
	uint8 co=0;
  uint8 *p1, *p2;

	if (flags&1) { x-=spr->w>>1; y-=spr->h>>1; }  // centered
	if (flags&2) { co=flags>>8; }  // colored
	if (flags&4) ik_drawbox(img, x, y, x+spr->w-1, y+spr->h-1, 0);

  if (x<c_minx-spr->w || y<c_miny-spr->h || x>=c_maxx || y>=c_maxy) return;

	yb=MAX(c_miny, y); ye=MIN(c_maxy, y+spr->h);
	xb=MAX(c_minx, x); xe=MIN(c_maxx, x+spr->w);

  for (py=yb; py<ye; py++)
  {
		px=xb;
    p1=ik_image_pointer(img,px,py); 
    p2=spr->data+((py-y)*spr->w)+(px-x);
		if (!co)
			for (;px<xe;px++)
      {
      	if (*p2)
					*p1=*p2;

        p1++;p2++;
      }
		else
  		for (;px<xe;px++)
      {
				if (*p2)
				{
      		if (*p2<16)
    	  	  *p1=*p2+co*16;
					else 
	        	*p1=*p2;
				}

        p1++;p2++;
      }
		}
}

// basic rsprite draw.. center align, rotation, scale (0-masked)
// flags:
// 1:  Light   (flags = 1 + lightcolor*256)
// 2:  Trans   (50% transparency)
// 4:  Add     
void ik_drsprite(t_ik_image *img, int32 x, int32 y, int32 r, int32 s, t_ik_sprite *spr, int32 flags)
{
  int32 x1,y1,x2;
	int32 size;
	int32 xt,yt,c,cx,cy;
	int32 cutleft, cutright;
	int32 dx,dy;
  uint8 *p1;

	if (s<=2) 
	{	
		c = spr->co;
		if (c)
		{
			if (y>=c_miny && x>=c_minx && x<c_maxx && y<c_maxy)
			{
				if (flags)
				{
			    p1=ik_image_pointer(img, x, y);

					if (flags&1)  c=gfx_lightbuffer[(c<<8)+(flags>>8)];
					if (flags&2)  c=gfx_transbuffer[(c<<8)+(*p1)];
					if (flags&4)  c=gfx_addbuffer[(c<<8)+(*p1)];

				  *p1=c;
				}
				else
					ik_putpixel(img, x,y,c);
			}
		}
		return;
	}

	s=(s<<10)/MAX(spr->w,spr->h);
  size=MAX(spr->w,spr->h)*s>>11; 

  if (x<c_minx-size || y<c_miny-size || x>=c_maxx+size || y>=c_maxy+size) return;

	r &= 1023;
	dx = cos1k[r]*1024/s;
	dy = -sin1k[r]*1024/s;
	cx=(spr->w+1)<<15;
	cy=(spr->h+1)<<15;

	size=(int32)(size*1.4);

  for (y1=-size; y1<size; y1++)
  {
  	if (y1+y>=c_miny && y1+y<c_maxy)
    {
      x2=x+size;if (x2>c_maxx) x2=c_maxx;
			x1=x-size;if (x1<c_minx) x1=c_minx;
			xt=cx+(x1-x)*dx-y1*dy;
			yt=cy+y1*dx+(x1-x)*dy;

			cutleft=0; cutright=0;
			// Clamp X
			if (dx>0)
			{
				if (xt+(x2-x1)*dx>spr->w<<16) 	cutright=MAX(cutright, (xt+(x2-x1)*dx-(spr->w<<16))/dx); 
				if (xt<0)												cutleft=MAX(cutleft, -xt/dx+1); 
			}
			else if (dx<0)
			{
				if (xt+(x2-x1)*dx<0)			cutright=MAX(cutright, (xt+(x2-x1)*dx)/dx); 
				if (xt>spr->w<<16)				cutleft=MAX(cutleft, -(xt-(spr->w<<16))/dx+1);
			}
			else if (xt<0 || xt>=spr->w<<16)	 x2=x1;   // don't draw hline

			// Clamp Y
			if (x2>x1)
			if (dy>0)
			{
				if (yt+(x2-x1)*dy>spr->h<<16)			cutright=MAX(cutright, (yt+(x2-x1)*dy-(spr->h<<16))/dy);
				if (yt<0)													cutleft=MAX(cutleft, -yt/dy+1);
			}
			else if (dy<0)
			{
				if (yt+(x2-x1)*dy<0)				cutright=MAX(cutright, (yt+(x2-x1)*dy)/dy); 
				if (yt>spr->h<<16)					cutleft=MAX(cutleft, -(yt-(spr->h<<16))/dy+1); 
			}
			else if (yt<0 || yt>=spr->h<<16)	x2=x1;  // don't draw hline

			// Apply clamps
			if (cutleft)
			{ xt+=dx*cutleft; yt+=dy*cutleft; x1+=cutleft; }
			if (cutright)
			{ x2-=cutright; }

      p1=ik_image_pointer(img, x1, y1+y);

			// innerloops
			if (!flags)  // "clean" .. no special fx .. fast
				for (;x1<x2;x1++)
		    {
					c=spr->data[(yt>>16)*spr->w+(xt>>16)];
					if (c)
					  *p1=c;

	        p1++;
					xt+=dx; yt+=dy;
				}
			else  // light, transparency, additive
				for (;x1<x2;x1++)
		    {
					c=spr->data[(yt>>16)*spr->w+(xt>>16)];
					if (c)
					{
						if (flags&1)  c=gfx_lightbuffer[(c<<8)+(flags>>8)];
						if (flags&2)  c=gfx_transbuffer[(c<<8)+(*p1)];
						if (flags&4)  c=gfx_addbuffer[(c<<8)+(*p1)];
					  *p1=c;
					}

	        p1++;
					xt+=dx; yt+=dy;
				}    
    }
  }
}

// sprite line draw.. line of tiled sprites (useful for laser beams etc)
// flags:
// 1:  Light   (flags = 1 + lightcolor*256)
// 2:  Trans   (50% transparency)
// 4:  Add     

void ik_dspriteline(t_ik_image *img, int32 xb, int32 yb, int32 xe, int32 ye, int32 s, 
										int32 offset, int32 ybits, t_ik_sprite *spr, int32 flags)
{
	double r;
  int32 x1,y1,x2;
	int32 size;
	int32 xt,yt,c,cx;
	int32 cutleft, cutright;
	int32 dx,dy;
	int32 xl0,yl0,xl1,yl1,topy;
  uint8 *p1;

	if (s<=2) 
	{	
		ik_drawline(img, xb,yb,xe,ye,spr->co);
		return;
	}

	s=(s<<6)/MAX(spr->w,spr->h);
  size=MAX(spr->w,spr->h)*s>>7; 

	xl0=MAX(c_minx, MIN(xb-size, xe-size));
	xl1=MIN(c_maxx, MAX(xb+size, xe+size));
	yl0=MAX(c_miny, MIN(yb-size, ye-size));
	yl1=MIN(c_maxy, MAX(yb+size, ye+size));

	if (xl0>xl1 || yl0>yl1) return;  // if clipped out
  if (xl1<c_minx || yl1<c_miny || xl0>=c_maxx || yl0>=c_maxy) return;

	r=atan2(xe-xb, yb-ye);
	dx=(int32)(cos(r)*65536*64/s);
	dy=-(int32)(sin(r)*65536*64/s);
	cx=(spr->w+1)<<15;

//	topy=-(int32)sqrt((xe-xb)*(xe-xb)+(ye-yb)*(ye-yb))*(65536*64/s);
	topy=(ye-yb)*dx+(xe-xb)*dy;

  for (y1=yl0; y1<yl1; y1++)
  {
  	if (y1>=c_miny && y1<c_maxy)
    {
      x2=xl1;if (x2>c_maxx) x2=c_maxx;
			x1=xl0;if (x1<c_minx) x1=c_minx;
			xt=cx+(x1-xb)*dx-(y1-yb)*dy;
			yt=(y1-yb)*dx+(x1-xb)*dy;

			cutleft=0; cutright=0;
			// Clamp X
			if (dx>0)
			{
				if (xt+(x2-x1)*dx>spr->w<<16) 	cutright=MAX(cutright, (xt+(x2-x1)*dx-(spr->w<<16))/dx); 
				if (xt<0)												cutleft=MAX(cutleft, -xt/dx+1); 
			}
			else if (dx<0)
			{
				if (xt+(x2-x1)*dx<0)			cutright=MAX(cutright, (xt+(x2-x1)*dx)/dx); 
				if (xt>spr->w<<16)				cutleft=MAX(cutleft, -(xt-(spr->w<<16))/dx+1);
			}
			else if (xt<0 || xt>=spr->w<<16)	x2=x1;  // don't draw hline

			// Clamp Y
			if (x2>x1)
			if (dy>0)
			{
				if (yt+(x2-x1)*dy>0)		cutright=MAX(cutright, (yt+(x2-x1)*dy)/dy);
				if (yt<topy)						cutleft=MAX(cutleft, (topy-yt)/dy+1);
			}
			else if (dy<0)
			{
				if (yt+(x2-x1)*dy<topy)		cutright=MAX(cutright, -(topy-(yt+(x2-x1)*dy))/dy+1); 
				if (yt>0)									cutleft=MAX(cutleft, -yt/dy+1); 
			}
			else if (yt<topy || yt>=0)	 x2=x1;   // don't draw hline

			// Apply clamps
			if (cutleft)
			{ xt+=dx*cutleft; yt+=dy*cutleft; x1+=cutleft; }
			if (cutright)
			{ x2-=cutright; }

      p1=ik_image_pointer(img, x1, y1);

			// innerloops
			if (!flags)  // "clean" .. no special fx .. fast
				for (;x1<x2;x1++)
		    {
					c=spr->data[(((yt>>ybits)+offset)&(spr->h-1))*spr->w+(xt>>16)];
					if (c)
					  *p1=c;

	        p1++;
					xt+=dx; yt+=dy;
				}
			else  // light, transparency, additive
				for (;x1<x2;x1++)
		    {
					c=spr->data[(((yt>>ybits)+offset)&(spr->h-1))*spr->w+(xt>>16)];
					if (c)
					{
						if (flags&1)  c=gfx_lightbuffer[(c<<8)+(flags>>8)];
						if (flags&2)  c=gfx_transbuffer[(c<<8)+(*p1)];
						if (flags&4)  c=gfx_addbuffer[(c<<8)+(*p1)];
					  *p1=c;
					}

	        p1++;
					xt+=dx; yt+=dy;
				}    
    }
  }
}

