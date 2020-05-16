#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

#include "typedefs.h"
#include "iface_globals.h"
#include "gfx.h"
#include "is_fileio.h"
#include "interface.h"

//#define THICK_MAGNIFIER

// GLOBALS

t_ik_image *magni;
t_ik_image *screen;
t_ik_image screenbuf;
int gfx_width, gfx_height, gfx_fullscreen, gfx_switch;
int gfx_redraw;
int c_minx, c_miny, c_maxx, c_maxy;

#ifdef MOVIE
int aframe = 0;
#endif

unsigned char *gfx_transbuffer;
unsigned char *gfx_lightbuffer;
unsigned char *gfx_addbuffer;

t_ik_image *dims[8];
int num_dims;

uint8 globalpal[768];
uint8 currentpal[768];

int32 sin1k[1024];
int32 cos1k[1024];

// PUTPIXEL/GETPIXEL

void ik_setclip(int32 left, int32 top, int32 right, int32 bottom)
{
	c_minx = left;
	c_miny = top;
	c_maxx = right;
	c_maxy = bottom;
}

void ik_putpixel(t_ik_image *img, int32 x, int32 y, uint32 c)
{
	img->data[y*img->pitch+x]=c;
}

void ik_putpixel_add(t_ik_image *img, int32 x, int32 y, uint32 c)
{
	img->data[y*img->pitch+x]=gfx_addbuffer[img->data[y*img->pitch+x]+(c<<8)];
}

int32 ik_getpixel(t_ik_image *img, int32 x, int32 y)
{
	return img->data[y*img->pitch+x];
}

uint8 *ik_image_pointer(t_ik_image *img, int32 x, int32 y)
{
	return img->data+y*img->pitch+x;
}

void ik_drawline(t_ik_image *img, int32 xb, int32 yb, int32 xe, int32 ye, int32 c1, int32 c2, uint8 mask, uint8 fx)
{
	int32 x=xb<<16,y=yb<<16;
	int32 dx,dy,d,x1,y1;
	int32 cutb=0, cute=0;

  dx=xe-xb; dy=ye-yb;

  if (dx==0 && dy==0) return;

	x1=abs(dx); y1=abs(dy);
  if (x1>y1)
  {
  	d=abs(dx)+1;
  	dy=(dy<<16)/x1;
    dx=(dx<<16)/x1;
  }
  else
  {
  	d=abs(dy)+1;
    dx=(dx<<16)/y1;
  	dy=(dy<<16)/y1;
  }

	if (dx>0)  // clamp x
	{
		if (x<(c_minx<<16)) { cutb=MAX(((c_minx<<16)-x)/dx,cutb); }
		if (x+dx*d>(c_maxx<<16)-1) { cute=MAX((x+dx*d-(c_maxx<<16)+1)/dx, cute); }
	}
	else if (dx<0)
	{
		if (x>(c_maxx<<16)-1) { cutb=MAX((x-(c_maxx<<16)+1)/dx, cutb); }
		if (x+dx*d<(c_minx<<16)) { cute=MAX(((c_minx<<16)-x-dx*d)/dx, cute); }
	}
	else if (x>>16<c_minx || x>>16>=c_maxx)  return;

//  ik_putpixel(img, x>>16,y>>16,c1); 


	if (dy>0)  // clamp y
	{
		if (y<(c_minx<<16)) { cutb=MAX(((c_miny<<16)-y)/dy,cutb); }
		if (y+dy*d>(c_maxy<<16)-1) { cute=MAX((y+dy*d-(c_maxy<<16)+1)/dy, cute); }
	}
	else if (dy<0)
	{
		if (y>(c_maxy<<16)-1) { cutb=MAX((y-(c_maxy<<16)+1)/dy, cutb); }
		if (y+dy*d<(c_miny<<16)) { cute=MAX(((c_miny<<16)-y-dy*d)/dy, cute); }
	}
	else if (y>>16<c_miny || y>>16>=c_maxy)  return;

	if (cutb) { x+=dx*cutb; y+=dy*cutb; d-=cutb; }
	if (cute) { d-=cute; }

	if (d<0) d=0;
  while (d--)
  {
  	if ( (1<<(d&7)) & mask )  // check mask
    {
  		x1=x>>16;y1=y>>16;
	    if (x1>=c_minx && y1>=c_miny && x1<c_maxx && y1<c_maxy)
				if (!fx)
					ik_putpixel(img, x1,y1,c1); 
				else
					ik_putpixel_add(img, x1,y1,c1); 
   	}
    else if (c2>0)
    {
  		x1=x>>16;y1=y>>16;
	    if (x1>=c_minx && y1>=c_miny && x1<c_maxx && y1<c_maxy)
				if (!fx)
					ik_putpixel(img, x1,y1,c2); 
				else
					ik_putpixel_add(img, x1,y1,c2); 
   	}

  	x+=dx; y+=dy;
  }
}

void ik_drawbox(t_ik_image *img, int32 xb, int32 yb, int32 xe, int32 ye, int32 c)
{
	int32 y;
	int32 w;
	uint8 *p;

	if (xe < xb) { w = xe; xe = xb; xb = w; }
	if (ye < yb) { w = ye; ye = yb; yb = w; }

	y = MAX(yb, c_miny);
	p = img->data + y*img->pitch + MAX(xb, c_minx);
	w = MIN(xe+1, c_maxx) - MAX(xb, c_minx);

	while (y < MIN(ye+1,c_maxy))
	{
		memset(p, c, w);
		y++; p += img->pitch;
	}

	/*
	for (y=MAX(yb,c_miny); y<MIN(ye+1,c_maxy); y++)
		for (x=MAX(xb,c_minx); x<MIN(xe+1,c_maxx); x++)
		{
			ik_putpixel(img, x, y, c);
		}
	*/
}

void ik_copybox(t_ik_image *src, t_ik_image *dst, int32 xb, int32 yb, int32 xe, int32 ye, int32 xd, int32 yd)
{
	int32 y;

	for (y=0;y<ye-yb;y++)
		memcpy(ik_image_pointer(dst,xd,y+yd),ik_image_pointer(src,xb,y+yb),xe-xb);
}

void ik_drawmeter(t_ik_image *img, int32 xb, int32 yb, int32 xe, int32 ye, int32 typ, int32 val, int32 c, int32 c2)
{
	ik_drawbox(img, xb, yb, xe, ye, c*16+12);
	ik_drawbox(img, xb+1, yb+1, xe-1, ye-1, c*16+1);
	if (typ==0) // vert
	{
		val = (val * (ye-yb-2)) / 100;
		ik_drawbox(img, xb+1, ye-1-val, xe-1, ye-1, c2);
	}
	else // horiz
	{
		val = (val * (xe-xb-2)) / 100;
		ik_drawbox(img, xb+1, yb+1, xb+1+val, ye-1, c2);
	}
}

// FIND RGB COLOR

int32 get_rgb_color(int32 r, int32 g, int32 b)
{
	int32 c,e,ee,x,r1,g1,b1;
	int32 c0;

	c=0;ee=200000;
	for (x=0;x<256;x++)
	{
		c0=get_palette_entry(x);
		r1=r-((c0>>16)&255); 
		g1=g-((c0>>8)&255); 
		b1=b-(c0&255);
		e=r1*r1+g1*g1+b1*b1;
		if (e<ee)
		{ c=x; ee=e; }
	}
	
	return c;
}

// CALCULATE COLOR TABLES

void calc_color_tables(uint8 *pal)
{
	int32 x,y;
	FILE *colormap;

	gfx_addbuffer=(unsigned char*)malloc(65536);
	gfx_transbuffer=(unsigned char*)malloc(65536);
	gfx_lightbuffer=(unsigned char*)malloc(65536);

	if (gfx_transbuffer==NULL || gfx_lightbuffer==NULL || gfx_addbuffer==NULL)
		return;  // fail

	colormap=myopen("graphics/colormap.dat", "rb");
	if (colormap)
	{
		fread(gfx_transbuffer, 1, 65536, colormap);
		fread(gfx_lightbuffer, 1, 65536, colormap);
		fread(gfx_addbuffer, 1, 65536, colormap);
		fclose(colormap);
		return;
	}

	for (y=0;y<256;y++)
		for (x=0;x<256;x++)
		{			
			gfx_transbuffer[y*256+x]=get_rgb_color( ((int32)pal[y*3]+pal[x*3])>>1,
																							((int32)pal[y*3+1]+pal[x*3+1])>>1,
																							((int32)pal[y*3+2]+pal[x*3+2])>>1 );
			gfx_lightbuffer[y*256+x]=get_rgb_color( ((int32)pal[y*3]*pal[x*3])>>8,
																							((int32)pal[y*3+1]*pal[x*3+1])>>8,
																							((int32)pal[y*3+2]*pal[x*3+2])>>8 );
			gfx_addbuffer[y*256+x]=get_rgb_color( MIN((int32)pal[y*3]+pal[x*3],255),
																						MIN((int32)pal[y*3+1]+pal[x*3+1],255),
																						MIN((int32)pal[y*3+2]+pal[x*3+2],255) );
		}

	colormap=myopen("graphics/colormap.dat","wb");

	fwrite(gfx_transbuffer, 1, 65536, colormap);
	fwrite(gfx_lightbuffer, 1, 65536, colormap);
	fwrite(gfx_addbuffer, 1, 65536, colormap);

	fclose(colormap);
}

void del_color_tables()
{
	if (gfx_transbuffer)  free(gfx_transbuffer);
	if (gfx_lightbuffer)  free(gfx_lightbuffer);
	if (gfx_addbuffer)  free(gfx_addbuffer);
}

// GENERATE OR LOAD IMAGE STRUCTS

t_ik_image *new_image(int32 w, int32 h)
{
	t_ik_image *img;

	img=(t_ik_image*)calloc(1, sizeof(t_ik_image));
	if (!img)
	{
		return NULL;
	}

	img->w=w;
	img->h=h;
	img->pitch=w;

	//calloc
	img->data=(uint8*)calloc(w*h, sizeof(uint8));
	if (!img->data)
	{
		free(img);
		return NULL;
	}

	return img;
}

void del_image(t_ik_image *img)
{
	if (!img)
		return;

	if (img->data)
		free(img->data);

	free(img);
}

t_ik_image *ik_load_pcx(char *fname, uint8 *pal)
{
	int32 x,y,po;
	int32 c,ch;
	uint16 img_w, img_h, line_w;
	uint8 bpp;

	t_ik_image *image;
	uint8 *buffer;
	uint8 *line;

	FILE *img;

	img=myopen(fname, "rb");
	if (!img)
		return NULL;

	// load header
	fgetc(img);  // manuf. id
	fgetc(img);  // version
  fgetc(img);  // encoding

	bpp=fgetc(img);
	// 4	

	img_w=-fgetc(img);  // xmin
	img_w-=fgetc(img)<<8;
	img_h=-fgetc(img);  // ymin
	img_h-=fgetc(img)<<8;
	img_w+=1+fgetc(img);  // xmax    (width is xmax-xmin)
	img_w+=fgetc(img)<<8;
	img_h+=1+fgetc(img);  // ymax
	img_h+=fgetc(img)<<8;
	// 12

	fgetc(img); // hdpi
	fgetc(img);
	fgetc(img); // vdpi
	fgetc(img);
	// 16

	for (x=0;x<48;x++)
		fgetc(img);
	// 64

	fgetc(img);
	bpp=fgetc(img)*8;

	line_w=fgetc(img);
	line_w+=fgetc(img)<<8;

	for (x=0;x<60;x++)
		fgetc(img);
	// 128

	if (bpp!=8)  // can't load non-8bit pcx files ... use tga
	{
		fclose(img);
		return NULL;
	}

	// read palette from the end
	if (pal)
	{
		fseek(img, -768, SEEK_END);
		fread(pal, 1, 768, img);
	}

	// allocate buffer for the image
	image=new_image(img_w, img_h);
	if (!image)
	{
		fclose(img);	
		return NULL;
	}

	buffer=image->data;
	line=(uint8*)malloc(line_w);

	// read image data
	fseek(img, 128, SEEK_SET);

	for (y=0;y<img_h;y++)
	{
		x=0;
		while (x<line_w)
		{
			ch=fgetc(img);
			if (ch >= 0xc0)
			{
				c=ch & 0x3f;
				ch=fgetc(img);
			}
			else
				c=1;

			while (c>0 && x<line_w)
			{
				line[x++]=ch;
				c--;
			}			
		}

		po=y*img_w;
		for (x=0;x<line_w;x++)
			image->data[po++]=line[x];
	}

	fclose(img);

	free(line);
	
	return image;					
}

t_ik_image *ik_load_tga(char *fname, uint8 *pal) 
{
	t_ik_image *img;
	FILE *fil;
	int p;
	uint8 hdr[18];
	
	fil = fopen(fname, "rb");
	if (!fil) return NULL;

	fread(hdr, 1, 18, fil);
	p = 1;
	if (hdr[1] != 1) p = 0;
	if (hdr[2] != 1) p = 0;
	if (hdr[7] != 24) p = 0;
	if (hdr[16] != 8) p = 0;
//	if (hdr[17] != 8) p = 0;

	if (!p)
	{
		fclose(fil);
		printf("ERROR: Bad TGA format %s", fname);
		return NULL;
	}

	// read palette
	if (pal)
	{
		for (p = 0; p < 256; p++)
		{
			pal[p*3+2] = fgetc(fil);
			pal[p*3+1] = fgetc(fil);
			pal[p*3] = fgetc(fil);
		}
	}
	else
		fseek(fil, 768, SEEK_CUR);

	// read image data
	img = new_image(hdr[13]*256+hdr[12], hdr[15]*256+hdr[14]);
	if (!img)
	{ fclose(fil);	return NULL; }

	for (p = img->h; p > 0; p--)
	{
		fread(img->data + (p-1)*img->pitch, 1, img->w, fil);
	}

	fclose(fil);

	return img;
}


void ik_save_screenshot(t_ik_image *img, uint8 *pal)
{
	int n;
	FILE *fil;
	char fname[32];
	
	wants_screenshot = 0;

#ifdef MOVIE
	n = aframe;
	aframe++;
	sprintf(fname, "frames/fram%04d.tga", n);
	ik_save_tga(fname, img, pal);
#else
	
	n=0;
	while (n<1000)
	{
		sprintf(fname, "shot%04d.tga", n);
		fil = myopen(fname, "rb");
		if (!fil)
		{
			ik_save_tga(fname, img, pal);
			break;
		}
		else fclose(fil);

		n++;
	}
#endif
}

void ik_save_tga(char *fname, t_ik_image *img, uint8 *pal)
{
	int p;
	uint8 hdr[18] = {
		0, 1, 1,								// id_len, pal_type, img_type
		0, 0, 0, 1,	24,					// first_color, num_colors, pal_size
		0, 0, 0, 0,							// left, top
		img->w&255, img->w>>8,	// width
		img->h&255, img->h>>8,	// height
		8, 8										// bpp, des_bits
		};
	FILE *fil;

	fil = myopen(fname, "wb");
	if (!fil)
		return;

	// write header
	fwrite(hdr, 1, 18, fil);

	// write palette
	for (p = 0; p < 256; p++)
	{
		fputc(get_palette_entry(p)&255, fil);
		fputc((get_palette_entry(p)>>8)&255, fil);
		fputc(get_palette_entry(p)>>16, fil);
	}

	// write data
	for (p = img->h; p > 0; p--)
	{
		fwrite(img->data + (p-1)*img->pitch, 1, img->w, fil);
	}

	fclose(fil);

}

int get_direction(int32 dx, int32 dy)
{
	int32 a;

	if (dx==0 && dy==0)
		return 0;

	a = (int32)(atan2(dx, dy)*512/3.14159);
	a = (a + 1024) & 1023;

	return a;
}

int get_distance(int32 dx, int32 dy)
{
	int32 r;

	if (dx==0 && dy==0)
		return 0;

	r = (int32)sqrt(dx*dx + dy*dy);

	return r;
}

void halfbritescreen()
{
	int32 x, y;
	int32 l;
	uint8 *po;

	if (num_dims >= 8)
		return;

	prep_screen();
	dims[num_dims] = new_image(screen->w, screen->h);
	if (!dims[num_dims])
	{	free_screen(); return; }

	ik_copybox(screen, dims[num_dims], 0, 0, screen->w, screen->h, 0, 0);
	num_dims++;

	if (num_dims > 1)
		l = 15;
	else
		l = 11;

	l<<=8;

	for (y = 0; y < screen->h; y++)
	{
		po = screen->data + screen->pitch*y;
		for (x = screen->w; x; x--)
		{	
			*po = gfx_lightbuffer[l+*po];
			po++;
		}
	}
	free_screen();
}

void reshalfbritescreen()
{
	if (num_dims <= 0)
		return;

	prep_screen();

	num_dims--;
	ik_copybox(dims[num_dims], screen, 0, 0, screen->w, screen->h, 0, 0);
	del_image(dims[num_dims]);
	dims[num_dims]=NULL;

	free_screen();
}

void resallhalfbritescreens()
{
	if (num_dims <= 0)
		return;

	prep_screen();

	while (num_dims)
	{
		num_dims--;
		ik_copybox(dims[num_dims], screen, 0, 0, screen->w, screen->h, 0, 0);
		del_image(dims[num_dims]);
		dims[num_dims]=NULL;
	}
	free_screen();
}

extern t_ik_spritepak *spr_IFbutton;

void ik_draw_mousecursor()
{
	ik_dsprite(screen, ik_mouse_x, ik_mouse_y, spr_IFbutton->spr[2], 0);
}

extern t_ik_spritepak *spr_SMraces;

void gfx_blarg()
{
	ik_dsprite(screen, 564, 448, spr_SMraces->spr[7], 0);
}

void gfx_initmagnifier()
{
	int x, y, r;
#ifndef THICK_MAGNIFIER
	int p;
#endif

	magni = new_image(128, 128);
	for (y = 0; y < 128; y++)
		for (x = 0; x < 128; x++)
		{
			r = (int)sqrt ( (y+y-127)*(y+y-127) + (x+x-127)*(x+x-127) );
#ifdef THICK_MAGNIFIER
			if (r < 124)
				magni->data[y*128+x] = 1;
			else if (r < 128)
				magni->data[y*128+x] = 2;
			else 
				magni->data[y*128+x] = 0;
#else
			if (r < 124)
				magni->data[y*128+x] = 1;
			else 
				magni->data[y*128+x] = 0;
#endif
		}
#ifndef THICK_MAGNIFIER
	p = 0;
	for (y = 0; y < 128; y++)
		for (x = 0; x < 128; x++)
		{
			if (!magni->data[p])
			{
				r = 0;
				if (y > 0)
					if (magni->data[p-128] == 1)
						r++;
				if (y < 127)
					if (magni->data[p+128] == 1)
						r++;
				if (x > 0)
					if (magni->data[p-1] == 1)
						r++;
				if (x < 127)
					if (magni->data[p+1] == 1)
						r++;
				if (r)
					magni->data[p]=2;
			}
			p++;
		}
#endif
}

void gfx_deinitmagnifier()
{
	del_image(magni);
}

void gfx_magnify()
{
	t_ik_sprite *mag;
	int  y;	// x
	unsigned char *p;
	//unsigned char *m;

//	mag = get_sprite(screen, ik_mouse_x-64, ik_mouse_y-64, 128, 128);
	mag = get_sprite(screen, ik_mouse_x-96, ik_mouse_y-48, 192, 96);
	p = mag->data;
	y = mag->h * mag->w;
	while (y--)
	{
		if (!*p)
			*p = 16;
		p++;
	}
/*
	p = mag->data;
	m = magni->data;
	for (y = 0; y < mag->h; y++)
		for (x = 0; x < mag->w; x++)
		{
			//if (y == 0 || x == 0 || y == mag->h-1 || x == mag->w-1)
			//	*p = 178;
			//else

			if (*m == 1)
			{
				if (!*p)
					*p = 16;
			}
			else if (*m == 2)
			{
				*p = 178;
			}
			else
				*p = *m;
			
			p++; m++;
		}

	ik_drsprite(screen, ik_mouse_x+1, ik_mouse_y+1, 0, 256, mag, 0);
	*/
	ik_drsprite(screen, ik_mouse_x+1, ik_mouse_y+1, 0, 384, mag, 0);
	interface_thinborder(screen, ik_mouse_x-192, ik_mouse_y-96, ik_mouse_x+192, ik_mouse_y+96, 11, -1);
	/*
	ik_drawline(screen, ik_mouse_x-192, ik_mouse_y-96, ik_mouse_x+191, ik_mouse_y-96, 178, 0, 255, 0);
	ik_drawline(screen, ik_mouse_x-192, ik_mouse_y+95, ik_mouse_x+191, ik_mouse_y+95, 178, 0, 255, 0);
	ik_drawline(screen, ik_mouse_x-192, ik_mouse_y-96, ik_mouse_x-192, ik_mouse_y+95, 178, 0, 255, 0);
	ik_drawline(screen, ik_mouse_x+191, ik_mouse_y-96, ik_mouse_x+191, ik_mouse_y+95, 178, 0, 255, 0);
	*/
	free_sprite(mag);
}