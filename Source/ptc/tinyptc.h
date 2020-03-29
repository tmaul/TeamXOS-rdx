//
// TinyPTC by Gaffer
// www.gaffer.org/tinyptc
//

#ifndef __TINYPTC_XBOX_H
#define __TINYPTC_XBOX_H

// tinyptc api
extern int ptc_open(char *title,int width,int height);
extern int ptc_update(void *buffer);
extern void ptc_close();

//xbox specific 
LPDIRECT3DTEXTURE8 ptc_gettexture();

// endian configuration
#define __PTC_LITTLE_ENDIAN__

// optimization
#define __PTC_MMX__
#define __PTC_MEMCPY__

#endif


