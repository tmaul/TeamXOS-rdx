#pragma once
#include <xtl.h>
#define XFONT_TRUETYPE // use true type fonts
#include <xgraphics.h>
#include <xfont.h>



int Font_Init();
void Font_Exit();
int Font_Output(WCHAR *text,unsigned int colour,int dx,int dy);