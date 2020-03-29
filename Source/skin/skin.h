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

#pragma once

#include "xbfont.h"
#include "xbresource.h"
#include "xbutil.h"

#define menu_titlecol 0xFFFFFF7f
#define itemcol 0xFFFFFFFF
//#define sel_itemcol 0xFF7F7FFF
#define sel_itemcol 0xFFFF6c6c

#define sel_bg 0xFF6f6f90

//#define menu_framecol 0xfffbfbfb
#define menu_framecol 0xff90b590
#define menu_bgcol 0xbf6f90b5

void Skin_Init();
void Skin_Info();
void Skin_BottomBar();
void Skin_BG();

void Skin_MainMenu();
void Skin_Emumenu();
void Skin_Fileman();
void Skin_Saveman();

void Skin_Exit();
