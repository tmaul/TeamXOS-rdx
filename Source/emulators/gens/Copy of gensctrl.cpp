#include "..\app.h"
#include "..\panel.h"
#include "sndxbox.hxx"
#include "io.h"
#include "vdp_io.h"
#include "blit.h"
#include "save.h"
#include "Mem_M68k.h" 

SoundXBOX m_sound ;
char				g_savePath[500] ;
char				g_saveprefix[500] ;
char				g_sramfile[500] ;

extern int Gens_Running ;
extern bool gens_render;

int GensInit(char *romfilename, int country);
int GensFrame();
int GensRender();
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

	for (y=0; y<ymax; y++,line+=lock.Pitch)
	{
		int x=0;
		int addr=line;

		for (x=0; x<xmax; x++,addr+=2)
		{
			*(unsigned int *)(dest+addr)=*ps++;
		}
	}
	gensTexture->UnlockRect(0);
	return 0;
}

int render_to_texture()
{
	int Dep=0;
	bool offset=0;
	
	// Get a description of our level 0 texture so we can figure
	// out the pitch of the texture
	D3DSURFACE_DESC desc;
    gensTexture->GetLevelDesc(0, &desc);
	
	if ((VDP_Reg.Set4 & 0x1) )
	{
		//width = 320 ;
		Dep = 0;
		offset = 0;
	}
	else
	{

		//width = 256
		Dep = 0;
		offset = 64;
	}
	
	// Allocate a buffer to blit our frames to

	// Figure out how big of a rect to lock in our texture
	RECT rectSource;
	rectSource.top = 0;
	rectSource.left = 0;
	rectSource.bottom = (VDP_Num_Vis_Lines<<1)  - 1 ;
	rectSource.right  = ((320-Dep) << 1 ) - 1 ;

	// Lock the rect in our texture
	D3DLOCKED_RECT d3dlr;
	gensTexture->LockRect(0, &d3dlr, &rectSource, 0);
	Blit_X2_MMX(g_pBlitBuff, d3dlr.Pitch, 320-Dep, VDP_Num_Vis_Lines, 32 + Dep * 2 ) ;
	gensTexture->UnlockRect(0);
	TexScreenSwizzle(rectSource.right+1,rectSource.bottom);
	// Unlock our texture

    g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

	g_pd3dDevice->BeginScene();
	gens_RenderPanel.Render(offset,0,640,480,1,0);
	//writexbox("render_to_textur exit\n");
	return 1;
}

void handleEvents()
{
	XBInput_GetInput();

// player 1

	if ( g_Gamepads[0].hDevice )
	{
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

	}
	else
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

	}
//player 2 - untested. :)
	if ( g_Gamepads[1].hDevice )
	{

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

	}
	else
	{
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
	}

	return ;
}

int gensinittexture()
{
	gens_RenderPanel.Destroy();
	g_pd3dDevice->CreateTexture(640,480, 1, 0, D3DFMT_LIN_R5G6B5 , NULL, &gensTexture);
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

void InitGens(char * romfilename, int country)
{

	g_pBlitBuff = NULL ;
	gensTexture = NULL ;
	if( FAILED( DirectSoundCreate( NULL, &(m_sound.dsound),  NULL ) ) )
		return;
	
	m_sound.dsound_init() ;
	m_sound.m_fps = 60;//m_d3dpp.FullScreen_RefreshRateInHz ;
	m_sound.init() ;
	m_sound.pause( FALSE ) ;

	gensinittexture();
	GensInit(romfilename, country);
}


void ExitGens()
{
	delete [] g_pBlitBuff;
	g_pBlitBuff = NULL;
	GensExit();
	gens_RenderPanel.Destroy();
	//m_sound.dsound_destroy_buffers();
	m_sound.cleanup();

}

void RenderGens()
{
	//handleEvents();
//	if ( gens_render )
	{
		render_to_texture();
		gens_render=0;
	}
	while ( m_sound.m_bDanger )
	{
		handleEvents();
	}
}

#ifdef __cplusplus
extern "C" {
#endif


void xbox_pause_audio( bool state ) 
{
	m_sound.pause( state ) ;
}

char* xbox_get_sramfile()
{
	return g_sramfile ;
}

char* xbox_get_savepath()
{
	return g_savePath ;
}

int xbox_check_events()
{
	handleEvents();
	return 0;
}

void xbox_process_audio()
{
	m_sound.process();
}

#ifdef __cplusplus
}
#endif

