#include "../emu_ctrl.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "fceu.h"
#include "types.h"
#include "vidblit.h"
#include "driver.h"

	int UsrInputType[3]={SI_GAMEPAD,SI_GAMEPAD,SIFC_NONE};
	int InputType[3];
	static int cspec;

	int gametype;


	static unsigned short joy1;
	static unsigned short joy2;

	unsigned short color_palette[256];

#define NWIDTH	(256-16)
#define NOFFSET	(8)

	CPanel nes_RenderPanel; 
	LPDIRECT3DTEXTURE8 nesTexture;
	FCEUGI *CurGame=NULL;

	static uint8 *gfx;  

	static int isloaded=0;

	static int soundrate=44100;
	static int soundbuftime=60;
	static int soundoptions=0;
	static int soundvolume=100;
	static int soundquality=0;

	static int tlines=0;
	static int myPitch=0;

	static int ntsccol=0,ntschue=0,ntsctint=0;
	static int srendline,erendline;
	static int srendlinen=8;
	static int erendlinen=231;
	static int srendlinep=0;
	static int erendlinep=239;
	static int sponge=0;
	static void DriverKill(void);
	static int DriverInitialize(FCEUGI *gi);




	static void DriverKill(void)
	{
		//SaveConfig();
		KillBlitToHigh();
		nes_RenderPanel.Destroy();
		/* #ifndef WIN32
		SetSignals(SIG_IGN);
		#endif

		if(inited&2)
		KillJoysticks();
		if(inited&8)
		KillKeyboard();
		if(inited&4)
		KillVideo();
		if(inited&1)
		KillSound();
		if(inited&16)
		KillMouse();
		inited=0;*/
	}



	static int DriverInitialize(FCEUGI *gi)
	{
		/* Initialize video before all else, due to some wacko dependencies
		in the SexyAL code(DirectSound) that need to be fixed.
		*/
		FCEUI_GetCurrentVidSystem(&srendline,&erendline);
		tlines=erendline-srendline+1;
		InitBlitToHigh(16>>3,0x1F<<11,0x3F<<5,0x1F,0,0);

		nes_RenderPanel.Destroy();
		g_pd3dDevice->CreateTexture(256,tlines, 1, 0, D3DFMT_LIN_R5G6B5 , NULL, &nesTexture);
		nes_RenderPanel.Create(g_pd3dDevice,nesTexture, true);
		D3DSURFACE_DESC desc;
		nesTexture->GetLevelDesc(0, &desc);
		D3DLOCKED_RECT d3dlr;
		nesTexture->LockRect(0, &d3dlr, NULL, 0);

		myPitch = d3dlr.Pitch ;
		nesTexture->UnlockRect(0);

		FCEUI_Sound(44100);
		FCEUI_SetSoundVolume(100);
		FCEUI_SetSoundQuality(1);


		/*
		if(!InitVideo(gi)) return 0;
		inited|=4;

		if(InitSound(gi))
		inited|=1;

		if(InitJoysticks())
		inited|=2;

		if(!InitKeyboard()) return 0;
		inited|=8;

		InitOtherInput();*/
		return 1;
	}


	FILE *FCEUD_UTF8fopen(const char *n, const char *m)
	{
		return(fopen(n,m));
	}

	void FCEUD_PrintError(char *s)
	{
	}

	void FCEUD_NetworkClose(void)
	{

	}

	void FCEUD_Message(char *s)
	{

	}

	static void FixPaletteHi(void)
	{

		//SetPaletteBlitToHigh((uint8 *)color_palette);
	}

	void FCEUD_SetPalette(unsigned char index, unsigned char r, unsigned char g, unsigned char b)
	{
		unsigned int t;
		t =(r<<8)&0xf800; // rrrr r000 0000 0000
		t|=(g<<3)&0x07e0; // 0000 0ggg ggg0 0000
		t|=(b>>3)&0x001f; // 0000 0000 000b bbbb

		color_palette[index]=t;
	}

	void FCEUD_GetPalette(unsigned char i, unsigned char *r, unsigned char *g, unsigned char *b)
	{
		*r = (color_palette[i] >> 11) & 0xFF;
		*g = (color_palette[i] >> 5) & 0xFF;
		*b = (color_palette[i]) & 0xFF;
	}


	int FCEUD_SendData(void *data, uint32 len)
	{
		return(1);
	} 


	/* Closes a game.  Frees memory, and deinitializes the drivers. */
	int CloseGame(void)
	{
		if(!isloaded) 
			return(0);
		FCEUI_CloseGame();
		DriverKill();
		isloaded=0;
		CurGame=0;
		return(1);
	}

	int LoadGame(const char *path)
	{
		FCEUGI *tmp;

		CloseGame();
		if(!(tmp=FCEUI_LoadGame(path)))
			return 0;
		CurGame=tmp;
		if(!DriverInitialize(tmp))
			return(0);  
		isloaded=1;
		return 1;
	}

	void BlitScreen(uint8 *XBuf)
	{

		int xo=0,yo=0;
		XBuf+=srendline*256;

		D3DLOCKED_RECT lock={0,NULL};
		unsigned char *dest=NULL;
		int y=0,line=0;
		uint8 *ps=NULL;
		nesTexture->LockRect(0,&lock,NULL,0); 
		if (lock.pBits==NULL) 
			return;

		dest=(unsigned char *)lock.pBits;
		ps=XBuf;

		for (y=0; y<tlines; y++,line+=lock.Pitch)
		{
			int x=0;
			int addr=line;

			for (x=0; x<256; x++,addr+=2)
			{
				int hold=*ps++;
				*(unsigned int *)(dest+addr)=color_palette[hold];//*(uint16 *)src];
			}
		}
		nesTexture->UnlockRect(0);
	}


	void FCEUD_UpdateInput(void)
	{
		joy1=0;
		joy2=0;

		if (g_Gamepads[0].bAnalogButtons[ XINPUT_GAMEPAD_A ]) 
			joy1 |= JOY_A ;

		if (g_Gamepads[0].bAnalogButtons[ XINPUT_GAMEPAD_B ] ) 
			joy1 |= JOY_B ;

		if (g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
			joy1 |= JOY_LEFT ;

		if (g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
			joy1 |= JOY_RIGHT ;

		if (g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_UP ) 
			joy1 |= JOY_UP ;

		if (g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
			joy1 |= JOY_DOWN ;

		if (g_Gamepads[0].wButtons & XINPUT_GAMEPAD_START )  
			joy1 |= JOY_START ;

		if (g_Gamepads[0].wButtons & XINPUT_GAMEPAD_BACK )  
			joy1 |= JOY_SELECT ;

		if (g_Gamepads[1].bAnalogButtons[ XINPUT_GAMEPAD_A ] ) 
			joy2 |= JOY_A ;

		if (g_Gamepads[1].bAnalogButtons[ XINPUT_GAMEPAD_B ] ) 
			joy2 |= JOY_B ;

		if ( g_Gamepads[1].wButtons & XINPUT_GAMEPAD_DPAD_LEFT ) 
			joy2 |= JOY_LEFT ;

		if ( g_Gamepads[1].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ) 
			joy2 |= JOY_RIGHT ;

		if (g_Gamepads[1].wButtons & XINPUT_GAMEPAD_DPAD_UP ) 
			joy2 |= JOY_UP ;

		if (g_Gamepads[1].wButtons & XINPUT_GAMEPAD_DPAD_DOWN ) 
			joy2 |= JOY_DOWN ;

		if (g_Gamepads[1].wButtons & XINPUT_GAMEPAD_START )  
			joy2 |= JOY_START ;

		if (g_Gamepads[1].wButtons & XINPUT_GAMEPAD_BACK )  
			joy2 |= JOY_SELECT ;
	}


	void FCEUD_Update(uint8 *XBuf, int32 *Buffer, int Count)
	{/*
	 if(Count)
	 {
	 int32 can=GetWriteSound();
	 static int uflow=0;
	 int32 tmpcan;

	 if(can >= GetMaxSound()) 
	 uflow=1;	

	 if(can > Count) 
	 can=Count;
	 else 
	 uflow=0;

	 WriteSound(Buffer,can);

	 //if(uflow) puts("Underflow");
	 tmpcan = GetWriteSound();
	 if((tmpcan < Count*0.90) && !uflow)
	 {
	 if(XBuf && (inited&4) && !(NoWaiting & 2))
	 BlitScreen(XBuf);
	 Buffer+=can;
	 Count-=can;
	 if(Count)
	 {
	 if(NoWaiting)
	 {
	 can=GetWriteSound(); 
	 if(Count>can) Count=can;
	 }
	 WriteSound(Buffer,Count);
	 }
	 } //else puts("Skipped");
	 }
	 else
	 {
	 if(!NoWaiting && !(eoptions&EO_NOTHROTTLE))
	 SpeedThrottle();
	 if(XBuf && (inited&4))
	 {
	 BlitScreen(XBuf);
	 }
	 }*/
		BlitScreen(XBuf);
		FCEUD_UpdateInput();
		memcpy(DSoundNext,(short*)Buffer,DSoundSeg*4 );
	}

	void DoFun(void)
	{
		int32 *sound;
		int32 ssize;
		FCEUI_Emulate(&gfx, &sound, &ssize, 0);
		FCEUD_Update(gfx, sound, ssize);

	}   


#ifdef __cplusplus
}
#endif

void InitUFC(char * romfilename)
{
	FCEUI_Initialize();
	LoadGame(romfilename);
	FCEUI_SetVidSystem(1);
	FCEUI_SetGameGenie(0);
	FCEUI_SetLowPass(0);
	FCEUI_SetInput(0, SI_GAMEPAD,&joy1,0);
	FCEUI_SetInput(1, SI_GAMEPAD,&joy2,0);
	FCEUI_DisableSpriteLimitation(1);
	FCEUI_SetSnapName(12);
	FCEUI_SetRenderedLines(srendlinen,erendlinen,srendlinep,erendlinep);
	FCEUI_GetNTSCTH(&ntsctint, &ntschue);
	FCEUI_SetNTSCTH(ntsccol, ntsctint, ntschue);
}

void ExitUFC()
{
	CloseGame();
	FCEUI_Kill();
}

void FrameUFC()
{
	DoFun();
}

void RenderUFC()
{
	nes_RenderPanel.Render(32,24,576,432,1,0);
}

