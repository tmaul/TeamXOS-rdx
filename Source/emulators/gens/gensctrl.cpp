#include "..\..\dash.h"
#include "io.h"
#include "vdp_io.h"
#include "blit.h"
#include "save.h"
#include "Mem_M68k.h" 
#include "g_dsound.h"
#include "misc.h"

extern int Gens_Running ;

int GensInit(char *romfilename, int country);
int GensExit();

static LPDIRECT3DTEXTURE8	gensTexture=NULL;
static byte*				g_pBlitBuff;
CPanel gens_RenderPanel; //used for rendering the screen

int TexScreenSwizzle(int xmax, int ymax)
{
    D3DLOCKED_RECT lock={0,NULL};
	unsigned char *dest=NULL;
	int y=0,line=0;
	unsigned short *ps=NULL;

	gensTexture->LockRect(0,&lock,NULL,0); 
	if (lock.pBits==NULL) 
		return 1;

	dest=(unsigned char *)lock.pBits;
	ps=(unsigned short*)g_pBlitBuff;

	for (y=0; y<240; y++,line+=lock.Pitch)
	{
		int x=0;
		int addr=line;

		for (x=0; x<320; x++,addr+=2)
		{
			*(unsigned int *)(dest+addr)=*ps++;
		}
	}
	gensTexture->UnlockRect(0);
	return 0;
}

void handleEvents()
{
	Controller_1_Left = 1 ;
	Controller_1_Right = 1 ;
	Controller_1_Up = 1 ;
	Controller_1_Down = 1 ;
	Controller_1_A = 1 ;
	Controller_1_B = 1 ;
	Controller_1_C = 1 ;
	Controller_1_X = 1 ;
	Controller_1_Y = 1 ;
	Controller_1_Z = 1 ;
	Controller_1_Start = 1 ;
	Controller_1_Mode = 1 ;
	Controller_1_A = ! ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_X]);
	Controller_1_B = ! ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_A]);
	Controller_1_C = ! ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_B]);
	Controller_1_X = ! ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_Y]);
	Controller_1_Y = ! ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_WHITE]);
	Controller_1_Z = ! ( g_Gamepads[0].bAnalogButtons[XINPUT_GAMEPAD_BLACK]);
	Controller_1_Start	= ! ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_START);
	Controller_1_Mode	= ! ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_BACK);

	if ( g_Gamepads[0].fX1 || g_Gamepads[0].fY1 )
	{
		Controller_1_Left = ( g_Gamepads[0].fX1 >= 0 ) ;
		Controller_1_Right = ( g_Gamepads[0].fX1 <= 0 ) ;
		Controller_1_Up = ( g_Gamepads[0].fY1 <= 0 ) ;
		Controller_1_Down = ( g_Gamepads[0].fY1 >= 0 ) ;
	}
	else
	{
		Controller_1_Left = ( ! ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) ) ;
		Controller_1_Right = ( ! ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) ) ;
		Controller_1_Up = ( ! ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_UP ) ) ;
		Controller_1_Down = ( ! ( g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) ) ;
	}


	Controller_2_Left = 1 ;
	Controller_2_Right = 1 ;
	Controller_2_Up = 1 ;
	Controller_2_Down = 1 ;
	Controller_2_A = 1 ;
	Controller_2_B = 1 ;
	Controller_2_C = 1 ;
	Controller_2_X = 1 ;
	Controller_2_Y = 1 ;
	Controller_2_Z = 1 ;
	Controller_2_Start = 1 ;
	Controller_2_Mode = 1 ;

	Controller_2_A = ! ( g_Gamepads[1].bAnalogButtons[XINPUT_GAMEPAD_X]);
	Controller_2_B = ! ( g_Gamepads[1].bAnalogButtons[XINPUT_GAMEPAD_A]);
	Controller_2_C = ! ( g_Gamepads[1].bAnalogButtons[XINPUT_GAMEPAD_B]);
	Controller_2_X = ! ( g_Gamepads[1].bAnalogButtons[XINPUT_GAMEPAD_Y]);
	Controller_2_Y = ! ( g_Gamepads[1].bAnalogButtons[XINPUT_GAMEPAD_WHITE]);
	Controller_2_Z = ! ( g_Gamepads[1].bAnalogButtons[XINPUT_GAMEPAD_BLACK]);

	Controller_2_Start	= ! ( g_Gamepads[1].wButtons & XINPUT_GAMEPAD_START);
	Controller_2_Mode	= ! ( g_Gamepads[1].wButtons & XINPUT_GAMEPAD_BACK);


	if ( g_Gamepads[0].fX1 || g_Gamepads[1].fY1 )
	{
		Controller_2_Left = ( g_Gamepads[1].fX1 >= 0 ) ;
		Controller_2_Right = ( g_Gamepads[1].fX1 <= 0 ) ;
		Controller_2_Up = ( g_Gamepads[1].fY1 <= 0 ) ;
		Controller_2_Down = ( g_Gamepads[1].fY1 >= 0 ) ;

	}
	else
	{
		Controller_2_Left = ( ! ( g_Gamepads[1].wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) ) ;
		Controller_2_Right = ( ! ( g_Gamepads[1].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) ) ;
		Controller_2_Up = ( ! ( g_Gamepads[1].wButtons & XINPUT_GAMEPAD_DPAD_UP ) ) ;
		Controller_2_Down = ( ! ( g_Gamepads[1].wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) ) ;

	}

	return ;
}

int gensinittexture()
{
	gens_RenderPanel.Destroy();
	g_pd3dDevice->CreateTexture(320,240, 1, 0, D3DFMT_LIN_R5G6B5 , NULL, &gensTexture);
	gens_RenderPanel.Create(g_pd3dDevice,gensTexture, true);
	D3DSURFACE_DESC desc;
    gensTexture->GetLevelDesc(0, &desc);

	if (g_pBlitBuff != NULL)
	{
		delete [] g_pBlitBuff;
		g_pBlitBuff = NULL;
	}
	g_pBlitBuff = new byte[desc.Size];

	return 0;
}

void InitGens(char * romfilename)
{
	g_pBlitBuff = NULL ;
	gensTexture = NULL ;
	gensinittexture();
	GensInit(romfilename, 0);
}


void FrameGens()
{
}

void ExitGens()
{
	delete [] g_pBlitBuff;
	g_pBlitBuff = NULL;
	GensExit();
	gens_RenderPanel.Destroy();
}

void RenderGens()
{
	bool offset=0;
	
	// Get a description of our level 0 texture so we can figure
	// out the pitch of the texture
	D3DSURFACE_DESC desc;
    gensTexture->GetLevelDesc(0, &desc);
	
	if ((VDP_Reg.Set4 & 0x1) )
	{
		offset = 0;
	}
	else
	{
		offset = 64;
	}
	
	Blit_X1_MMX(g_pBlitBuff, 320*2, 320, VDP_Num_Vis_Lines, 32 ) ;
	TexScreenSwizzle(320,240);
	gens_RenderPanel.Render(32,24,576,432,1,0);

}

#ifdef __cplusplus
extern "C" {
#endif

int xbox_check_events()
{
	handleEvents();
	return 0;
}

void xbox_process_audio()
{
	Write_Sound_Stereo_MMX(Seg_L, Seg_R, (short *) DSoundNext, DSoundSeg);
}

#ifdef __cplusplus
}
#endif

