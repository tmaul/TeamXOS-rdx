
//
// TinyPTC by Gaffer
// www.gaffer.org/tinyptc
//

//quick and duurty xbox port by kev. 
//Hardcoded to use 32 bit textures - why would use want to use anything else? :0) 
#include <xtl.h>
#include "tinyptc.h"
#include "mmx.h"

static int dx;
static int dy;

static LPDIRECT3DTEXTURE8	ptc_texture;

/*
example usage:

#define WIDTH 50
#define HEIGHT 50
#define SIZE WIDTH*HEIGHT

static int pixel[SIZE];

void init()
{
...
ptc_open("unused string",WIDTH,HEIGHT); // open PTC with height and texture
...
}

void frame()
{
.... // write some shit to pixel[]
ptc_update(pixel);  // make PTC update its texture with pixels stored in pixel[]
renderquad(ptc_gettexture()); // get PTC's texture and pass it into users function (needed on xbox only)
....
}

void close()
{
..
ptc_close(); // free ptc texture
..
}
*/

//create a ptc texture with width + height
int ptc_open(char *title,int width,int height)
{
	dx = width;
	dy = height;
	D3DDevice::CreateTexture(width,height, 1, 0, D3DFMT_LIN_X8R8G8B8 , NULL, &ptc_texture);
	return 1;
}

//buffer is an array of DWORD
int ptc_update(void *buffer)
{
	int y;
	unsigned char *src;
	unsigned char *dst;
	int src_pitch;
	int dst_pitch;
	D3DLOCKED_RECT lock={0,NULL};
	ptc_texture->LockRect(0,&lock,NULL,0); 
	if (lock.pBits==NULL) return 1;
	src_pitch = dx * 4;
	dst_pitch =	lock.Pitch;
	// copy pixels to textures surface
	src = (unsigned char*) buffer;
	dst = (unsigned char*) lock.pBits;;
	for (y=0; y<dy; y++)
	{
		// convert line
		mmx_memcpy(dst,src,dx*4);
		src += src_pitch;
		dst += dst_pitch;
	}
	ptc_texture->UnlockRect(0);
	return 0;
}

void ptc_close()
{
	ptc_texture->Release();
}

LPDIRECT3DTEXTURE8 ptc_gettexture()
{
	return ptc_texture;
}