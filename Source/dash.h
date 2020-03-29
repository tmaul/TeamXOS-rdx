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
#include <xtl.h>
#include <xgraphics.h>
#include <xfont.h>

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "xkutils\xkcrc.h"
#include "xkutils\xkeeprom.h"
#include "xkutils\xkexports.h"
#include "xkutils\xkgeneral.h"
#include "xkutils\xkhdd.h"
#include "xkutils\xkrc4.h"
#include "xkutils\xksha1.h"
#include "xkutils\xkutils.h"

#include "system\dashmenu.h"
#include "system\filepath.h"
#include "system\font.h"
#include "system\input.h"
#include "system\mappath.h"
#include "system\panel.h"
#include "system\utils.h"

#include "tinyxml\tinystr.h"

#include "config\config.h"

#include "emulators\emu_ctrl.h"

#include "playlist\playlist.h"

#include "info\info.h"

#include "skin\skin.h"

#include "ptc\tinyptc.h"

#include "saveman\saveman.h"

#include "fileman\fileman.h"

extern Menu *MenusNew[8];

#define SAFE_DELETE(p)       { delete (p);     (p)=NULL; }
#define SAFE_DELETE_ARRAY(p) { delete[] (p);   (p)=NULL; }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

extern LPDIRECT3DDEVICE8  g_pd3dDevice;

extern CXBFont smallfont;
extern CXBFont bigfont;
