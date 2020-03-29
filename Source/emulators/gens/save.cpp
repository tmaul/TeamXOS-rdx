#include <xtl.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include "save.h"
#include "cpu_68k.h"

#include "z80.h"

#include "gens.h"
#include "G_main.h"
#include "G_ddraw.h"
#include "G_dsound.h"
#include "G_input.h"
#include "vdp_io.h"
#include "vdp_rend.h"
#include "rom.h"
#include "mem_M68K.h"
#include "mem_Z80.h"
#include "ym2612.h"
#include "psg.h"
#include "pcm.h"

#include "io.h"
#include "misc.h"



int Current_State = 0;
char State_Dir[1024] = "";
char SRAM_Dir[1024] = "";
unsigned char State_Buffer[MAX_STATE_FILE_LENGHT];;


int Change_File_S(char *Dest, char *Dir, char *Titre, char *Filter, char *Ext)
{
	return 0;
}


int Change_File_L(char *Dest, char *Dir, char *Titre, char *Filter, char *Ext)
{
	return 0;
}


int Change_Dir(char *Dest, char *Dir, char *Titre, char *Filter, char *Ext)
{
	return 0;
}


FILE *Get_State_File()
{
	char Name[2048];
	char Ext[5] = ".gs";
	Ext[3] = '0' + Current_State;
	strcpy(Name, State_Dir);
	strcat(Name, Rom_Name);
	strcat(Name, Ext);

	return fopen(Name, "rb");
}


void Get_State_File_Name(char *name)
{
	char Ext[5] = ".gs";
	Ext[3] = '0' + Current_State;
	strcpy(name, State_Dir);
	strcat(name, Rom_Name);
	strcat(name, Ext);
}



int Load_State(char *Name)
{
	FILE *f;
	unsigned char *buf;
	int len;

	if (Genesis_Started) len = GENESIS_STATE_FILE_LENGHT_EX;
	else if (SegaCD_Started) len = SEGACD_STATE_FILE_LENGHT;
	else if (_32X_Started) len = G32X_STATE_FILE_LENGHT;
	else return 0;

	buf = State_Buffer;

	if ((f = fopen(Name, "rb")) == NULL) return 0;

	memset(buf, 0, len);
	if (fread(buf, 1, len, f))
	{
		z80_Reset(&M_Z80);
		if (SegaCD_Started)
		{
		}

		if (_32X_Started)
		{
		}

		Import_Genesis(buf);
		buf += 0x22500;

		Flag_Clr_Scr = 1;
		CRam_Flag = 1;
		VRam_Flag = 1;

		sprintf(Str_Tmp, "STATE %d LOADED", Current_State);
		Put_Info(Str_Tmp, 2000);
	}

	fclose(f);

	return 1;
}


int Save_State(char *Name)
{
	FILE *f;
	unsigned char *buf;
	int len;

	buf = State_Buffer;

	if ((f = fopen(Name, "wb")) == NULL) return 0;

	if (Genesis_Started) len = GENESIS_STATE_FILE_LENGHT;
	else if (SegaCD_Started) len = SEGACD_STATE_FILE_LENGHT;
	else if (_32X_Started) len = G32X_STATE_FILE_LENGHT;

	memset(buf, 0, len);
	Export_Genesis(buf);
	buf += 0x22500;
	
	fwrite(State_Buffer, 1, len, f);
	fclose(f);

	sprintf(Str_Tmp, "STATE %d SAVED", Current_State);
	Put_Info(Str_Tmp, 2000);

	return 1;
}

void Import_Genesis(unsigned char *Data)
{
	unsigned char Reg_1[0x200], Version, *src;
	int i;

	VDP_Int = 0;
	DMAT_Lenght = 0;
	Version = Data[0x50];

	for(i = 0; i < 0x80; i++) CRam[i] = Data[i + 0x112];
	for(i = 0; i < 0x50; i++) VSRam[i] = Data[i + 0x192];
	for(i = 0; i < 0x2000; i++) Ram_Z80[i] = Data[i + 0x474];
	
	for(i = 0; i < 0x10000; i += 2)
	{
		Ram_68k[i + 0] = Data[i + 0x2478 + 1];
		Ram_68k[i + 1] = Data[i + 0x2478 + 0];
	}

	for(i = 0; i < 0x10000; i += 2)
	{
		VRam[i + 0] = Data[i + 0x12478 + 1];
		VRam[i + 1] = Data[i + 0x12478 + 0];
	}

	for(i = 0; i < 0x200; i++) Reg_1[i] = Data[i + 0x1E4];
	YM2612_Restore(Reg_1);

	if ((Version >= 2) && (Version < 4))
	{
		src = (unsigned char *) &Ctrl;
		for(i = 0; i < 7 * 4; i++) *src++ = Data[i + 0x30];

		Z80_State &= ~6;
		if (Data[0x440] & 1) Z80_State |= 2;
		if (Data[0x444] & 1) Z80_State |= 4;

		src = (unsigned char *) &Bank_Z80;
		for(i = 0; i < 4; i++) *src++ = Data[i + 0x448];

		src = (unsigned char *) &PSG_Save;
		for(i = 0; i < 8 * 4; i++) *src++ = Data[i + 0x224B8];
		PSG_Restore_State();
	}
	else if ((Version >= 4) || (Version == 0)) 		// New version compatible with Kega.
	{
		Z80_State &= ~6;

		if (Version == 4)
		{
			M_Z80.IM = Data[0x437];
			M_Z80.IFF.b.IFF1 = (Data[0x438] & 1) << 2;
			M_Z80.IFF.b.IFF2 = (Data[0x438] & 1) << 2;

			Z80_State |= (Data[0x439] & 1) << 1;
		}
		else
		{
			M_Z80.IM = 1;
			M_Z80.IFF.b.IFF1 = (Data[0x436] & 1) << 2;
			M_Z80.IFF.b.IFF2 = (Data[0x436] & 1) << 2;

			Z80_State |= ((Data[0x439] & 1) ^ 1) << 1;
			Z80_State |= ((Data[0x438] & 1) ^ 1) << 2;
		}

		src = (unsigned char *) &Ctrl;
		for(i = 0; i < 7 * 4; i++) *src++ = 0;

		Write_VDP_Ctrl(Data[0x40] + (Data[0x41] << 8));
		Write_VDP_Ctrl(Data[0x42] + (Data[0x43] << 8));

		Ctrl.Flag = Data[0x44];
		Ctrl.DMA = (Data[0x45] & 1) << 2;
		Ctrl.Address = Data[0x48] + (Data[0x49] << 8);
		
		src = (unsigned char *) &Bank_Z80;
		for(i = 0; i < 4; i++) *src++ = Data[i + 0x43C];

		if (Version >= 4)
		{
			for(i = 0; i < 8; i++) PSG_Save[i] = Data[i * 2 + 0x60] + (Data[i * 2 + 0x61] << 8);
			PSG_Restore_State();
		}
	}

	z80_Set_AF(&M_Z80, Data[0x404] + (Data[0x405] << 8));
	M_Z80.BC.w.BC = Data[0x408] + (Data[0x409] << 8);
	M_Z80.DE.w.DE = Data[0x40C] + (Data[0x40D] << 8);
	M_Z80.HL.w.HL = Data[0x410] + (Data[0x411] << 8);
	M_Z80.IX.w.IX = Data[0x414] + (Data[0x415] << 8);
	M_Z80.IY.w.IY = Data[0x418] + (Data[0x419] << 8);
	z80_Set_PC(&M_Z80, Data[0x41C] + (Data[0x41D] << 8));
	M_Z80.SP.w.SP = Data[0x420] + (Data[0x421] << 8);
	z80_Set_AF2(&M_Z80, Data[0x424] + (Data[0x425] << 8));
	M_Z80.BC2.w.BC2 = Data[0x428] + (Data[0x429] << 8);
	M_Z80.DE2.w.DE2 = Data[0x42C] + (Data[0x42D] << 8);
	M_Z80.HL2.w.HL2 = Data[0x430] + (Data[0x431] << 8);
	M_Z80.I = Data[0x434] & 0xFF;

	main68k_GetContext(&Context_68K);

	for(i = 0; i < 24; i++) Set_VDP_Reg(i, Data[0xFA + i]);
	
	src = (unsigned char *) &Context_68K.dreg[0];
	for(i = 0; i < 8 * 2 * 4; i++) *src++ = Data[0x80 + i];

	src = (unsigned char *) &Context_68K.pc;
	for(i = 0; i < 4; i++) *src++ = Data[0xC8 + i];

	src = (unsigned char *) &Context_68K.sr;
	for(i = 0; i < 2; i++) *src++ = Data[0xD0 + i];

	if ((Version >= 3) || (Version == 0))
	{
		if (Data[0xD1] & 0x20)
		{
			// Supervisor
			src = (unsigned char *) &Context_68K.asp;
			for(i = 0; i < 2; i++) *src++ = Data[0xD2 + i];
		}
		else
		{
			// User
			src = (unsigned char *) &Context_68K.asp;
			for(i = 0; i < 2; i++) *src++ = Data[0xD6 + i];
		}
	}

	main68k_SetContext(&Context_68K);
}


void Export_Genesis(unsigned char *Data)
{
	unsigned char Reg_1[0x200], *src;
	int i;

	while (DMAT_Lenght) Update_DMA();		// Be sure to finish DMA before save

	Data[0x00] = 'G';
	Data[0x01] = 'S';
	Data[0x02] = 'T';
	Data[0x03] = 0x40;
	Data[0x04] = 0xE0;

	Data[0x50] = 5;		// Version
	Data[0x51] = 0;		// Gens

	PSG_Save_State();

	for(i = 0; i < 8; i++)
	{
		Data[0x60 + i * 2] = PSG_Save[i] & 0xFF;
		Data[0x61 + i * 2] = (PSG_Save[i] >> 8) & 0xFF;
	}

	main68k_GetContext(&Context_68K);

	src = (unsigned char *) &(Context_68K.dreg[0]);
	for(i = 0; i < 8 * 2 * 4; i++) Data[0x80 + i] = *src++;

	src = (unsigned char *) &(Context_68K.pc);
	for(i = 0; i < 4; i++) Data[0xC8 + i] = *src++;

	src = (unsigned char *) &(Context_68K.sr);
	for(i = 0; i < 2; i++) Data[0xD0 + i] = *src++;

	if (Context_68K.sr & 0x2000)
	{
		src = (unsigned char *) &(Context_68K.asp);
		for(i = 0; i < 4; i++) Data[0xD2 + i] = *src++;
		src = (unsigned char *) &(Context_68K.areg[7]);
		for(i = 0; i < 4; i++) Data[0xD6 + i] = *src++;
	}
	else
	{
		src = (unsigned char *) &(Context_68K.asp);
		for(i = 0; i < 4; i++) Data[0xD6 + i] = *src++;
		src = (unsigned char *) &(Context_68K.areg[7]);
		for(i = 0; i < 4; i++) Data[0xD2 + i] = *src++;
	}

	src = (unsigned char *) &(Ctrl.Data);
	for(i = 0; i < 4; i++) Data[0x40 + i] = *src++;

	Data[0x44] = Ctrl.Flag;
	Data[0x45] = (Ctrl.DMA >> 2) & 1;

	Data[0x48] = Ctrl.Address & 0xFF;
	Data[0x49] = (Ctrl.Address >> 8) & 0xFF;

	VDP_Reg.DMA_Lenght_L = VDP_Reg.DMA_Lenght & 0xFF;
	VDP_Reg.DMA_Lenght_H = (VDP_Reg.DMA_Lenght >> 8) & 0xFF;

	VDP_Reg.DMA_Src_Adr_L = VDP_Reg.DMA_Address & 0xFF;
	VDP_Reg.DMA_Src_Adr_M = (VDP_Reg.DMA_Address >> 8) & 0xFF;
	VDP_Reg.DMA_Src_Adr_H = (VDP_Reg.DMA_Address >> 16) & 0xFF;

	VDP_Reg.DMA_Src_Adr_H |= Ctrl.DMA_Mode & 0xC0;

	src = (unsigned char *) &(VDP_Reg.Set1);
	for(i = 0; i < 24; i++)
	{
		Data[0xFA + i] = *src;
		src += 4;
	}

	for(i = 0; i < 0x80; i++) Data[i + 0x112] = CRam[i];
	for(i = 0; i < 0x50; i++) Data[i + 0x192] = VSRam[i];

	YM2612_Save(Reg_1);
	for(i = 0; i < 0x200; i++) Data[i + 0x1E4] = Reg_1[i];

	Data[0x404] = (unsigned char) (z80_Get_AF(&M_Z80) & 0xFF);
	Data[0x405] = (unsigned char) (z80_Get_AF(&M_Z80) >> 8);
	Data[0x408] = (unsigned char) (M_Z80.BC.w.BC & 0xFF);
	Data[0x409] = (unsigned char) (M_Z80.BC.w.BC >> 8);
	Data[0x40C] = (unsigned char) (M_Z80.DE.w.DE & 0xFF);
	Data[0x40D] = (unsigned char) (M_Z80.DE.w.DE >> 8);
	Data[0x410] = (unsigned char) (M_Z80.HL.w.HL & 0xFF);
	Data[0x411] = (unsigned char) (M_Z80.HL.w.HL >> 8);
	Data[0x414] = (unsigned char) (M_Z80.IX.w.IX & 0xFF);
	Data[0x415] = (unsigned char) (M_Z80.IX.w.IX >> 8);
	Data[0x418] = (unsigned char) (M_Z80.IY.w.IY & 0xFF);
	Data[0x419] = (unsigned char) (M_Z80.IY.w.IY >> 8);
	Data[0x41C] = (unsigned char) (z80_Get_PC(&M_Z80) & 0xFF);
	Data[0x41D] = (unsigned char) (z80_Get_PC(&M_Z80) >> 8);
	Data[0x420] = (unsigned char) (M_Z80.SP.w.SP & 0xFF);
	Data[0x421] = (unsigned char) (M_Z80.SP.w.SP >> 8);
	Data[0x424] = (unsigned char) (z80_Get_AF2(&M_Z80) & 0xFF);
	Data[0x425] = (unsigned char) (z80_Get_AF2(&M_Z80) >> 8);
	Data[0x428] = (unsigned char) (M_Z80.BC2.w.BC2 & 0xFF);
	Data[0x429] = (unsigned char) (M_Z80.BC2.w.BC2 >> 8);
	Data[0x42C] = (unsigned char) (M_Z80.DE2.w.DE2 & 0xFF);
	Data[0x42D] = (unsigned char) (M_Z80.DE2.w.DE2 >> 8);
	Data[0x430] = (unsigned char) (M_Z80.HL2.w.HL2 & 0xFF);
	Data[0x431] = (unsigned char) (M_Z80.HL2.w.HL2 >> 8);
	Data[0x434] = (unsigned char) (M_Z80.I);
	Data[0x436] = (unsigned char) (M_Z80.IFF.b.IFF1 >> 2);

	Data[0x438] = (unsigned char) (((Z80_State & 4) >> 2) ^ 1);
	Data[0x439] = (unsigned char) (((Z80_State & 2) >> 1) ^ 1);

	src = (unsigned char *) &Bank_Z80;
	for(i = 0; i < 4; i++) Data[0x43C + i] = *src++;

	for(i = 0; i < 0x2000; i++) Data[i + 0x474] = Ram_Z80[i];

	for(i = 0; i < 0x10000; i += 2)
	{
		Data[i + 0x2478 + 1] = Ram_68k[i + 0];
		Data[i + 0x2478 + 0] = Ram_68k[i + 1];
	}

	for(i = 0; i < 0x10000; i += 2)
	{
		Data[i + 0x12478 + 1] = VRam[i + 0];
		Data[i + 0x12478 + 0] = VRam[i + 1];
	}
}



int Save_Config(char *File_Name)
{
	return 1;
}


int Save_As_Config(HWND hWnd)
{
	return 0 ;
}

#define Conf_File ""

DWORD GetPrivateProfileString( LPCTSTR, LPCTSTR, LPCTSTR lpDefault, LPTSTR returned, DWORD nSize, LPCTSTR)
{
	strcpy( returned, lpDefault) ;
	return 0 ;
}

UINT GetPrivateProfileInt( LPCTSTR, LPCTSTR, INT ndef, LPCTSTR)
{
	return ndef ;
}

#ifdef __cplusplus
extern "C" {
#endif

int multiport =0;

#ifdef __cplusplus
}
#endif

int Load_Config(char *File_Name, void *Game_Active)
{
	CRam_Flag = 1;

	strcpy( Rom_Dir, "D:\\SGENROMS\\" ) ;
	strcpy( State_Dir,"") ;
	strcpy( SRAM_Dir, "") ;
	strcpy( IPS_Dir, Rom_Dir);

	File_Type_Index = 1 ;

	Current_State = 0 ;
	Language = 0 ;
	Intro_Style = 2 ;
	Effect_Color = 7 ;
	Sleep_Time = 0 ;
	Gens_Priority = 1 ;

	Mode_555 = 0 ;

	RMax_Level = 255;
	GMax_Level = 255;
	BMax_Level = 255;
	Contrast_Level = 100;
	Brightness_Level = 100;
	Greyscale = 0;
	Invert_Color = 0;

	Recalculate_Palettes();

	FS_VSync = 0;
	W_VSync = 0;
	Full_Screen = 1;
	Render_W = 0;
	Render_FS = 1;

	Set_Render(HWnd, Full_Screen, -1, 1);

	Stretch = 0;
	Blit_Soft = 0;
	Sprite_Over = 1;
	Frame_Skip = -1;

	Sound_Rate = 44100 ;
	Sound_Stereo = 1 ;

	Z80_State |= 1 ;

	if (Change_Sound(HWnd))
	{
		YM2612_Enable = 1;
		PSG_Enable = 1;
		DAC_Enable = 1;
		PCM_Enable = 1;
	}

	YM2612_Improv = 1;
	DAC_Improv = 0;
	PSG_Improv = 0;

	Country = -1 ;
	Country_Order[0] = 0;
	Country_Order[1] = 1;
	Country_Order[2] = 2;

	Check_Country_Order();

	Fast_Blur = 0;
	Show_FPS = 0;
	FPS_Style = 0;
	Show_Message = 1;
	Message_Style = 0;
	Show_LED = 1;
	Auto_Fix_CS = 0;
	Auto_Pause = 0;
	//CUR_DEV = 0;

	Controller_1_Type = GetPrivateProfileInt("Input", "P1.Type", 1, Conf_File);
	Controller_1B_Type = GetPrivateProfileInt("Input", "P1B.Type", 0, Conf_File);
	Controller_1C_Type = GetPrivateProfileInt("Input", "P1C.Type", 0, Conf_File);
	Controller_1D_Type = GetPrivateProfileInt("Input", "P1D.Type", 0, Conf_File);
	Controller_2_Type = GetPrivateProfileInt("Input", "P2.Type", 1, Conf_File);
	Controller_2B_Type = GetPrivateProfileInt("Input", "P2B.Type", 0, Conf_File);
	Controller_2C_Type = GetPrivateProfileInt("Input", "P2C.Type", 0, Conf_File);
	Controller_2D_Type = GetPrivateProfileInt("Input", "P2D.Type", 0, Conf_File);

	if ( multiport == 1)
	{
		Controller_1_Type = 0x11 ;
		Controller_1B_Type = 0x01 ;
		Controller_1C_Type = 0x01 ;
		Controller_1D_Type = 0x01 ;
	}
	else
		Controller_1_Type = 0x01 ;


	if ( multiport == 2 )
	{
		Controller_2_Type = 0x11 ;
		Controller_2B_Type = 0x01 ;
		Controller_2C_Type = 0x01 ;
		Controller_2D_Type = 0x01 ;
	}
	else
	{
		Controller_2_Type = 0x01 ;
	}

	Make_IO_Table();
	return 1;
}


int Load_As_Config(HWND hWnd, void *Game_Active)
{
	return 0 ;
}


int Load_SRAM(void)
{
	int bResult;
	HANDLE SRAM_File;
	char Name[2048];
	unsigned long Bytes_Read;
	return 1;
}


int Save_SRAM(void)
{
	HANDLE SRAM_File;
	int bResult, size_to_save, i;
	char Name[2048];
	unsigned long Bytes_Write;
	return 1;
}

