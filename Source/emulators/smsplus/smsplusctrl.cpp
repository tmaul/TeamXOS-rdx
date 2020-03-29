
#include "shared.h"
#include "..\..\dash.h"


static LPDIRECT3DTEXTURE8	smsTexture=NULL;
static byte*				sms_pBlitBuff;
CPanel sms_RenderPanel; //used for rendering the screen

#ifdef __cplusplus
extern "C" {
#endif

	/* Save or load SRAM */
	void system_manage_sram(uint8 *sram, int slot, int mode)
	{
		switch(mode)
		{
		case SRAM_SAVE:
			break;

		case SRAM_LOAD:
			/* No SRAM file, so initialize memory */
			memset(sram, 0x00, 0x8000);
			break;
		}
	}

#ifdef __cplusplus
}
#endif


int SMSInitTexture()
{
	sms_RenderPanel.Destroy();
	g_pd3dDevice->CreateTexture(256,193, 1, 0, D3DFMT_LIN_R5G6B5 , NULL, &smsTexture);
	sms_RenderPanel.Create(g_pd3dDevice,smsTexture, true);
	D3DSURFACE_DESC desc;
	smsTexture->GetLevelDesc(0, &desc);

	if (sms_pBlitBuff != NULL)
	{
		delete [] sms_pBlitBuff;
		sms_pBlitBuff = NULL;
	}
	sms_pBlitBuff = new byte[desc.Size+16];
	return 0;
}

int SMSScreenSwizzle()
{
	D3DLOCKED_RECT lock={0,NULL};
	unsigned char *dest=NULL;
	int line=0;
	int addr=0;
	unsigned short *ps=NULL;

	smsTexture->LockRect(0,&lock,NULL,0); 
	if (lock.pBits==NULL) 
		return 1;

	dest=(unsigned char *)lock.pBits;
	ps=(unsigned short*)sms_pBlitBuff;

	for (int y=0; y<192; y++,line+=lock.Pitch)
	{
		addr=line;
		for (int x=0; x<256; x++,addr+=2)
		{
			*(unsigned int *)(dest+addr)=*ps++;
		}
	}
	smsTexture->UnlockRect(0);
	return 0;
}

void InitSMS(char * romfilename)
{
	//	strcpy(game_name, romfilename);
	load_rom(romfilename);

	SMSInitTexture();
	memset(&bitmap, 0, sizeof(bitmap_t));
	bitmap.width  = 256;
	bitmap.height = 192;
	bitmap.depth  = 16;
	bitmap.granularity = (bitmap.depth >> 3);
	bitmap.pitch  = bitmap.width * bitmap.granularity;
	bitmap.data   = (uint8 *)sms_pBlitBuff;
	bitmap.viewport.x = 0;
	bitmap.viewport.y = 0;
	bitmap.viewport.w = 256;
	bitmap.viewport.h = 192;

	snd.fm_which = SND_YM2413;
	snd.fps = (sms.display == DISPLAY_NTSC) ? FPS_NTSC : FPS_PAL;
	snd.fm_clock = (sms.display == DISPLAY_NTSC) ? CLOCK_NTSC : CLOCK_PAL;
	snd.psg_clock = (sms.display == DISPLAY_NTSC) ? CLOCK_NTSC : CLOCK_PAL;
	snd.sample_rate =44100;
	snd.mixer_callback = NULL;

	/* Initialize the virtual console emulation */
	system_init();
	sms.territory = TERRITORY_EXPORT;
	sms.use_fm = 1;

	system_poweron();
}

void ExitSMS()
{
	system_poweroff();
	system_shutdown();
	sms_RenderPanel.Destroy();
	delete [] sms_pBlitBuff;
	sms_pBlitBuff = NULL;
}

void SMSControls ()
{
	input.system    = 0;
	input.pad[0]    = 0;
	input.pad[1]    = 0;
	input.analog[0] = 0x7F;
	input.analog[1] = 0x7F;

	/* Player #1 inputs */
	if(g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_UP)         input.pad[0] |= INPUT_UP;
	else
		if(g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_DOWN)       input.pad[0] |= INPUT_DOWN;
	if(g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_LEFT)       input.pad[0] |= INPUT_LEFT;
	else
		if(g_Gamepads[0].hDevice && g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)      input.pad[0] |= INPUT_RIGHT;
	if(g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[ XINPUT_GAMEPAD_B ])          input.pad[0] |= INPUT_BUTTON2;
	if(g_Gamepads[0].hDevice && g_Gamepads[0].bAnalogButtons[ XINPUT_GAMEPAD_A ])          input.pad[0] |= INPUT_BUTTON1;

	/* Player #2 inputs */
	if(g_Gamepads[1].hDevice && g_Gamepads[1].wButtons & XINPUT_GAMEPAD_DPAD_UP)      input.pad[1] |= INPUT_UP;
	else
		if(g_Gamepads[1].hDevice && g_Gamepads[1].wButtons & XINPUT_GAMEPAD_DPAD_DOWN)      input.pad[1] |= INPUT_DOWN;
	if(g_Gamepads[1].hDevice && g_Gamepads[1].wButtons & XINPUT_GAMEPAD_DPAD_LEFT)      input.pad[1] |= INPUT_LEFT;
	else
		if(g_Gamepads[1].hDevice && g_Gamepads[1].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)      input.pad[1] |= INPUT_RIGHT;
	if(g_Gamepads[1].hDevice && g_Gamepads[1].bAnalogButtons[ XINPUT_GAMEPAD_B ])      input.pad[1] |= INPUT_BUTTON2;
	if(g_Gamepads[1].hDevice && g_Gamepads[1].bAnalogButtons[ XINPUT_GAMEPAD_A ])      input.pad[1] |= INPUT_BUTTON1;

}

void FrameSMS()
{
	SMSControls();
	system_frame(false);
	memcpy(DSoundNext,snd.output[0],snd.buffer_size*2);
}

void DrawTexturedQuad2( LPDIRECT3DTEXTURE8 pTex, float Left, float Right, float Top, float Bottom )
{
	struct stQuadVerts {
		float x, y, z, rhw;
		float tu, tv;
	};

	stQuadVerts myQuadVerts[] = {
		{ Left,  Top,    0, 1, 0, 0 },
		{ Right, Top,    0, 1, 1, 0 },
		{ Left,  Bottom, 0, 1, 0, 1 },
		{ Left,  Bottom, 0, 1, 0, 1 },
		{ Right, Top,    0, 1, 1, 0 },
		{ Right, Bottom, 0, 1, 1, 1 },
	};

	D3DDevice::SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	D3DDevice::SetTexture( 0, pTex );
	D3DDevice::DrawVerticesUP( D3DPT_TRIANGLESTRIP, 6, &myQuadVerts, sizeof(stQuadVerts) );
}

void RenderSMS()
{
	SMSScreenSwizzle();
	//DrawTexturedQuad2(smsTexture,30, 50, 20, 50);
	sms_RenderPanel.Render(0,0,640,480,1,0);
}