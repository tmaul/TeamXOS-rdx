/////
//
//	This code is part of the rdx project. (c) Team Xos
//
//  Team xos grant you the right to use this code in any non commerical way,
//  under the condidtion that you release your source code with any compiled binaries
//
//  If you make any useful changes to the code you are encouraged to email to us
//  for inclusion in a future version. Full credit will be given.
//
//  rdxdev@gmail.com
//

#include <xtl.h>
#include <xgraphics.h>
#include <wchar.h>
#include "font.h"
#include <xfont.h>

static XFONT *Font=NULL;
extern LPDIRECT3DSURFACE8 g_pBackBuffer;

int Font_Init()
{
  XFONT_OpenDefaultFont(&Font); 
  if (Font==NULL) 
	  return 1;

  return 0;
}

void Font_Exit()
{
}

int Font_Output(WCHAR *text,unsigned int colour,int dx,int dy)
{
  if (Font==NULL || g_pBackBuffer==NULL) 
	  return 1;
  Font->SetTextColor(0);
  Font->TextOut(g_pBackBuffer,text,~0U,dx+2,dy+2);
  Font->SetTextColor(colour);
  Font->TextOut(g_pBackBuffer,text,~0U,dx,dy);
  return 0;
}
