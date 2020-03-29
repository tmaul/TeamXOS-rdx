#include <stdio.h>
#include "gens.h"
#include "G_main.h"
#include "G_ddraw.h"
#include "G_dsound.h"
#include "G_input.h"
#include "rom.h"
#include "mem_M68K.h"

#include "ym2612.h"
#include "psg.h"
#include "Cpu_68k.h"
#include "Cpu_Z80.h"

#include "z80.h"
#include "vdp_io.h"
#include "vdp_rend.h"
#include "io.h"
#include "misc.h"
#include "save.h"

#include "pcm.h"

int Debug;
int Frame_Skip;
int Frame_Number;
int DAC_Improv;
int RMax_Level;
int GMax_Level;
int BMax_Level;
int Contrast_Level;
int Brightness_Level;
int Greyscale;
int Invert_Color;

unsigned char CD_Data[1024];		// Used for hard reset to know the game name


int Round_Double(double val)
{
	if ((val - (double) (int) val) > 0.5) return (int) (val + 1);
	else return (int) val;
}


void Init_Tab(void)
{
	int x, y, dep;

	for(x = 0; x < 1024; x++)
	{
		for(y = 0; y < 64; y++)
		{
			dep = (x & 3) + (((x & 0x3FC) >> 2) << 8);
			dep += ((y & 7) << 2) + (((y & 0xF8) >> 3) << 5);
			dep >>= 1;
//			Cell_Conv_Tab[(x >> 1) + (y << 9)] = (unsigned short) dep;
		}
	}

	for(x = 0; x < 512; x++)
	{
		for(y = 0; y < 64; y++)
		{
			dep = (x & 3) + (((x & 0x1FC) >> 2) << 8);
			dep += ((y & 7) << 2) + (((y & 0xF8) >> 3) << 5);
			dep >>= 1;
		//	Cell_Conv_Tab[(x >> 1) + (y << 8) + 0x8000] = (unsigned short) (dep + 0x8000);
		}
	}

	for(x = 0; x < 256; x++)
	{
		for(y = 0; y < 64; y++)
		{
			dep = (x & 3) + (((x & 0xFC) >> 2) << 8);
			dep += ((y & 7) << 2) + (((y & 0xF8) >> 3) << 5);
			dep >>= 1;
		//	Cell_Conv_Tab[(x >> 1) + (y << 7) + 0xC000] = (unsigned short) (dep + 0xC000);
		}
	}

	for(x = 0; x < 256; x++)
	{
		for(y = 0; y < 32; y++)
		{
			dep = (x & 3) + (((x & 0xFC) >> 2) << 7);
			dep += ((y & 7) << 2) + (((y & 0xF8) >> 3) << 5);
			dep >>= 1;
		//	Cell_Conv_Tab[(x >> 1) + (y << 7) + 0xE000] = (unsigned short) (dep + 0xE000);
		//	Cell_Conv_Tab[(x >> 1) + (y << 7) + 0xF000] = (unsigned short) (dep + 0xF000);
		}
	}

	for(x = 0; x < 512; x++) Z80_M68K_Cycle_Tab[x] = (int) ((double) x * 7.0 / 15.0);
}


void Recalculate_Palettes(void)
{
	int i;
	int r, g, b;
	int rf, gf, bf;
	int bright, cont;

	for(r = 0; r < 0x10; r++)
	{
		for(g = 0; g < 0x10; g++)
		{
			for(b = 0; b < 0x10; b++)
			{
				rf = (r & 0xE) << 2;
				gf = (g & 0xE) << 2;
				bf = (b & 0xE) << 2;

				rf = (double) (rf) * ((double) (RMax_Level) / 224.0);
				gf = (double) (gf) * ((double) (GMax_Level) / 224.0);
				bf = (double) (bf) * ((double) (BMax_Level) / 224.0);

				// Compute colors here (64 levels)

				bright = Brightness_Level;
				bright -= 100;
				bright *= 32;
				bright /= 100;

				rf += bright;
				gf += bright;
				bf += bright;

				if (rf < 0) rf = 0;
				else if (rf > 0x3F) rf = 0x3F;
				if (gf < 0) gf = 0;
				else if (gf > 0x3F) gf = 0x3F;
				if (bf < 0) bf = 0;
				else if (bf > 0x3F) bf = 0x3F;

				cont = Contrast_Level;

				rf = (rf * cont) / 100;
				gf = (gf * cont) / 100;
				bf = (bf * cont) / 100;
		
				if (rf < 0) rf = 0;
				else if (rf > 0x3F) rf = 0x3F;
				if (gf < 0) gf = 0;
				else if (gf > 0x3F) gf = 0x3F;
				if (bf < 0) bf = 0;
				else if (bf > 0x3F) bf = 0x3F;

				if (Mode_555 & 1)
				{
					rf = (rf >> 1) << 10;
					gf = (gf >> 1) << 5;
				}
				else
				{
					rf = (rf >> 1) << 11;
					gf = (gf >> 0) << 5;
				}
				bf = (bf >> 1) << 0;

				Palette[(b << 8) | (g << 4) | r] = rf | gf | bf;
			}
		}
	}

	if (Greyscale)
	{
		for(i = 0; i < 0x1000; i++)
		{
			if (Mode_555 & 1)
			{
				r = ((Palette[i] >> 10) & 0x1F) << 1;
				g = ((Palette[i] >> 5) & 0x1F) << 1;
			}
			else
			{
				r = ((Palette[i] >> 11) & 0x1F) << 1;
				g = (Palette[i] >> 5) & 0x3F;
			}

			b = ((Palette[i] >> 0) & 0x1F) << 1;

			r = (r * unsigned int (0.30 * 65536.0)) >> 16;
			g = (g * unsigned int (0.59 * 65536.0)) >> 16;
			b = (b * unsigned int (0.11 * 65536.0)) >> 16;

			r = g = b = r + g + b;

			if (Mode_555 & 1)
			{
				r = (r >> 1) << 10;
				g = (g >> 1) << 5;
			}
			else
			{
				r = (r >> 1) << 11;
				g = (g >> 0) << 5;
			}

			b = (b >> 1) << 0;

			Palette[i] = r | g | b;
		}

	}

	if (Invert_Color)
	{
		for(i = 0; i < 0x1000; i++)
		{
			Palette[i] ^= 0xFFFF;
		}


	}

}


void Check_Country_Order(void)
{
	if ((Country_Order[0] == Country_Order[1]) || (Country_Order[0] == Country_Order[2]) || (Country_Order[1] == Country_Order[2]) || (Country_Order[0] == Country_Order[2])
		|| (Country_Order[0] > 2) || (Country_Order[0] < 0) || (Country_Order[1] > 2) || (Country_Order[1] < 0) || (Country_Order[2] > 2) || (Country_Order[2] < 0))
	{
		Country_Order[0] = 0;
		Country_Order[1] = 1;
		Country_Order[2] = 2;
	}
}

void Detect_Country_Genesis(void)
{
	int c_tab[3] = {4, 1, 8};
	int gm_tab[3] = {1, 0, 1};
	int cm_tab[3] = {0, 0, 1};
	int i, coun = 0;
	char c;
	
	if (!strnicmp((char *) &Rom_Data[0x1F0], "eur", 3)) coun |= 8;
	else if (!strnicmp((char *) &Rom_Data[0x1F0], "usa", 3)) coun |= 4;
	else if (!strnicmp((char *) &Rom_Data[0x1F0], "jap", 3)) coun |= 1;
	else for(i = 0; i < 4; i++)
	{
		c = toupper(Rom_Data[0x1F0 + i]);
		
		if (c == 'U') coun |= 4;
		else if (c == 'J') coun |= 1;
		else if (c == 'E') coun |= 8;
		else if (c < 16) coun |= c;
		else if ((c >= '0') && (c <= '9')) coun |= c - '0';
		else if ((c >= 'A') && (c <= 'F')) coun |= c - 'A' + 10;
	}

	if (coun & c_tab[Country_Order[0]])
	{
		Game_Mode = gm_tab[Country_Order[0]];
		CPU_Mode = cm_tab[Country_Order[0]];
	}
	else if (coun & c_tab[Country_Order[1]])
	{
		Game_Mode = gm_tab[Country_Order[1]];
		CPU_Mode = cm_tab[Country_Order[1]];
	}
	else if (coun & c_tab[Country_Order[2]])
	{
		Game_Mode = gm_tab[Country_Order[2]];
		CPU_Mode = cm_tab[Country_Order[2]];
	}
	else if (coun & 2)
	{
		Game_Mode = 0;
		CPU_Mode = 1;
	}
	else
	{
		Game_Mode = 1;
		CPU_Mode = 0;
	}

	if (Game_Mode)
	{
		if (CPU_Mode) Put_Info("Europe system (50 FPS)", 1500);
		else Put_Info("USA system (60 FPS)", 1500);
	}
	else
	{
		if (CPU_Mode) Put_Info("Japan system (50 FPS)", 1500);
		else Put_Info("Japan system (60 FPS)", 1500);
	}

	if (CPU_Mode)
	{
		VDP_Status |= 0x0001;
	}
	else
	{
		VDP_Status &= 0xFFFE;
	}
}







/*************************************/
/*              GENESIS              */
/*************************************/


void Init_Genesis_Bios(void)
{
	memset(Genesis_Rom, 0, 2 * 1024);
	Rom_Size =  2 * 1024;
	memcpy(Rom_Data, Genesis_Rom, 2 * 1024);
	Game_Mode = 0;
	CPU_Mode = 0;
	VDP_Num_Vis_Lines = 224;
	M68K_Reset(0);
	Z80_Reset();
	Reset_VDP();
	CPL_Z80 = Round_Double((((double) CLOCK_NTSC / 15.0) / 60.0) / 262.0);
	CPL_M68K = Round_Double((((double) CLOCK_NTSC / 7.0) / 60.0) / 262.0);
	VDP_Num_Lines = 262;
	VDP_Status &= 0xFFFE;
	YM2612_Init(CLOCK_NTSC / 7, Sound_Rate, YM2612_Improv);
	PSG_Init(CLOCK_NTSC / 15, Sound_Rate);
}


int Init_Genesis(struct Rom *MD_Rom)
{
	char Str_Err[256];

	Flag_Clr_Scr = 1;
	Debug = Paused = Frame_Number = 0;
	SRAM_Start = SRAM_End = SRAM_ON = SRAM_Write = 0;
	Controller_1_COM = Controller_2_COM = 0;

	if (!Kaillera_Client_Running)
	{
		if ((MD_Rom->Ram_Infos[8] == 'R') && (MD_Rom->Ram_Infos[9] == 'A') && (MD_Rom->Ram_Infos[10] & 0x40))
		{
			SRAM_Start = MD_Rom->Ram_Start_Adress & 0x0F80000;		// multiple de 0x080000
			SRAM_End = MD_Rom->Ram_End_Adress;
		}
		else
		{
			SRAM_Start = 0x200000;
			SRAM_End = 0x200000 + (64 * 1024) - 1;
		}

		if ((SRAM_Start > SRAM_End) || ((SRAM_End - SRAM_Start) >= (64 * 1024)))
			SRAM_End = SRAM_Start + (64 * 1024) - 1;

		if (Rom_Size <= (2 * 1024 * 1024))
		{
			SRAM_ON = 1;
			SRAM_Write = 1;
		}

		SRAM_Start &= 0xFFFFFFFE;
		SRAM_End |= 0x00000001;

//		sprintf(Str_Err, "deb = %.8X end = %.8X", SRAM_Start, SRAM_End);
//		MessageBox(NULL, Str_Err, "", MB_OK);

		if ((SRAM_End - SRAM_Start) <= 2) SRAM_Custom = 1;
		else SRAM_Custom = 0;

		Load_SRAM();
	}
	
	switch(Country)
	{
		default:
		case -1:
			Detect_Country_Genesis();
			break;

		case 0:
			Game_Mode = 0;
			CPU_Mode = 0;

			break;

		case 1:
			Game_Mode = 1;
			CPU_Mode = 0;
			break;

		case 2:
			Game_Mode = 1;
			CPU_Mode = 1;
			break;

		case 3:
			Game_Mode = 0;
			CPU_Mode = 1;
			break;
	}

	if ((CPU_Mode == 1) || (Game_Mode == 0))
		sprintf(Str_Err, "Gens - Megadrive : %s",MD_Rom->Rom_Name_W);
	else
		sprintf(Str_Err, "Gens - Genesis : %s",MD_Rom->Rom_Name_W);

//	SetWindowText(HWnd, Str_Err);

	VDP_Num_Vis_Lines = 224;
	Gen_Version = 0x20 + 0x0;	 	// Version de la megadrive (0x0 - 0xF)

	Byte_Swap(Rom_Data, Rom_Size);

	M68K_Reset(0);
	Z80_Reset();
	Reset_VDP();

	if (CPU_Mode)
	{
		CPL_Z80 = Round_Double((((double) CLOCK_PAL / 15.0) / 50.0) / 312.0);
		CPL_M68K = Round_Double((((double) CLOCK_PAL / 7.0) / 50.0) / 312.0);
		VDP_Num_Lines = 312;
		VDP_Status |= 0x0001;

		YM2612_Init(CLOCK_PAL / 7, Sound_Rate, YM2612_Improv);
		PSG_Init(CLOCK_PAL / 15, Sound_Rate);
	}
	else
	{
		CPL_Z80 = Round_Double((((double) CLOCK_NTSC / 15.0) / 60.0) / 262.0);
		CPL_M68K = Round_Double((((double) CLOCK_NTSC / 7.0) / 60.0) / 262.0);

		VDP_Num_Lines = 262;
		VDP_Status &= 0xFFFE;

		YM2612_Init(CLOCK_NTSC / 7, Sound_Rate, YM2612_Improv);
		PSG_Init(CLOCK_NTSC / 15, Sound_Rate);
	}

	if (Auto_Fix_CS) Fix_Checksum();

	if (Sound_Enable)
	{
		End_Sound();

		if (!Init_Sound(HWnd)) Sound_Enable = 0;
		else Play_Sound();
	}

	Last_Time = GetTickCount();
	New_Time = 0;
	Used_Time = 0;

	Update_Frame = Do_Genesis_Frame;
	Update_Frame_Fast = Do_Genesis_Frame_No_VDP;

	return 1;
}


void Reset_Genesis()
{
	Controller_1_COM = Controller_2_COM = 0;
	Paused = 0;

	if (Rom_Size <= (2 * 1024 * 1024))
	{
		SRAM_ON = 1;
		SRAM_Write = 1;
	}
	else
	{
		SRAM_ON = 0;
		SRAM_Write = 0;
	}

	M68K_Reset(0);
	Z80_Reset();
	Reset_VDP();
	YM2612_Reset();

	if (CPU_Mode) VDP_Status |= 1;
	else VDP_Status &= ~1;

	if (Auto_Fix_CS) Fix_Checksum();
}


int Do_VDP_Only()
{
	if ((CPU_Mode) && (VDP_Reg.Set2 & 0x8))	VDP_Num_Vis_Lines = 240;
	else VDP_Num_Vis_Lines = 224;

	for(VDP_Current_Line = 0; VDP_Current_Line < VDP_Num_Vis_Lines; VDP_Current_Line++)
		Render_Line();

	return(0);
}


int Do_Genesis_Frame_No_VDP(void)
{
	int *buf[2];
	int HInt_Counter;

	if ((CPU_Mode) && (VDP_Reg.Set2 & 0x8))	VDP_Num_Vis_Lines = 240;
	else VDP_Num_Vis_Lines = 224;

	YM_Buf[0] = PSG_Buf[0] = Seg_L;
	YM_Buf[1] = PSG_Buf[1] = Seg_R;
	YM_Len = PSG_Len = 0;

	Cycles_M68K = Cycles_Z80 = 0;
	Last_BUS_REQ_Cnt = -1000;
	main68k_tripOdometer();
	z80_Clear_Odo(&M_Z80);

//	Patch_Codes();

	VRam_Flag = 1;

	VDP_Status &= 0xFFF7;							// Clear V Blank
	if (VDP_Reg.Set4 & 0x2) VDP_Status ^= 0x0010;

	HInt_Counter = VDP_Reg.H_Int;		// Hint_Counter = step H interrupt

	for(VDP_Current_Line = 0; VDP_Current_Line < VDP_Num_Vis_Lines; VDP_Current_Line++)
	{
		buf[0] = Seg_L + Sound_Extrapol[VDP_Current_Line][0];
		buf[1] = Seg_R + Sound_Extrapol[VDP_Current_Line][0];
		YM2612_DacAndTimers_Update(buf, Sound_Extrapol[VDP_Current_Line][1]);
		YM_Len += Sound_Extrapol[VDP_Current_Line][1];
		PSG_Len += Sound_Extrapol[VDP_Current_Line][1];

		Fix_Controllers();
		Cycles_M68K += CPL_M68K;
		Cycles_Z80 += CPL_Z80;
		if (DMAT_Lenght) main68k_addCycles(Update_DMA());

		VDP_Status |= 0x0004;					// HBlank = 1
//		main68k_exec(Cycles_M68K - 436);
		main68k_exec(Cycles_M68K - 404);
		VDP_Status &= 0xFFFB;					// HBlank = 0

		if (--HInt_Counter < 0)
		{
			HInt_Counter = VDP_Reg.H_Int;
			VDP_Int |= 0x4;
			Update_IRQ_Line();
		}

		main68k_exec(Cycles_M68K);
		if (Z80_State == 3) z80_Exec(&M_Z80, Cycles_Z80);
		else z80_Set_Odo(&M_Z80, Cycles_Z80);
	}
	
	buf[0] = Seg_L + Sound_Extrapol[VDP_Current_Line][0];
	buf[1] = Seg_R + Sound_Extrapol[VDP_Current_Line][0];
	YM2612_DacAndTimers_Update(buf, Sound_Extrapol[VDP_Current_Line][1]);
	YM_Len += Sound_Extrapol[VDP_Current_Line][1];
	PSG_Len += Sound_Extrapol[VDP_Current_Line][1];

	Fix_Controllers();
	Cycles_M68K += CPL_M68K;
	Cycles_Z80 += CPL_Z80;
	if (DMAT_Lenght) main68k_addCycles(Update_DMA());

	if (--HInt_Counter < 0)
	{
		VDP_Int |= 0x4;
		Update_IRQ_Line();
	}

	VDP_Status |= 0x000C;			// VBlank = 1 et HBlank = 1 (retour de balayage vertical en cours)
	main68k_exec(Cycles_M68K - 360);
	if (Z80_State == 3) z80_Exec(&M_Z80, Cycles_Z80 - 168);
	else z80_Set_Odo(&M_Z80, Cycles_Z80 - 168);

	VDP_Status &= 0xFFFB;			// HBlank = 0
	VDP_Status |= 0x0080;			// V Int happened
	VDP_Int |= 0x8;
	Update_IRQ_Line();
	z80_Interrupt(&M_Z80, 0xFF);

	main68k_exec(Cycles_M68K);
	if (Z80_State == 3) z80_Exec(&M_Z80, Cycles_Z80);
	else z80_Set_Odo(&M_Z80, Cycles_Z80);

	for(VDP_Current_Line++; VDP_Current_Line < VDP_Num_Lines; VDP_Current_Line++)
	{
		buf[0] = Seg_L + Sound_Extrapol[VDP_Current_Line][0];
		buf[1] = Seg_R + Sound_Extrapol[VDP_Current_Line][0];
		YM2612_DacAndTimers_Update(buf, Sound_Extrapol[VDP_Current_Line][1]);
		YM_Len += Sound_Extrapol[VDP_Current_Line][1];
		PSG_Len += Sound_Extrapol[VDP_Current_Line][1];

		Fix_Controllers();
		Cycles_M68K += CPL_M68K;
		Cycles_Z80 += CPL_Z80;
		if (DMAT_Lenght) main68k_addCycles(Update_DMA());

		VDP_Status |= 0x0004;					// HBlank = 1
//		main68k_exec(Cycles_M68K - 436);
		main68k_exec(Cycles_M68K - 404);
		VDP_Status &= 0xFFFB;					// HBlank = 0

		main68k_exec(Cycles_M68K);
		if (Z80_State == 3) z80_Exec(&M_Z80, Cycles_Z80);
		else z80_Set_Odo(&M_Z80, Cycles_Z80);
	}

	PSG_Special_Update();
	YM2612_Special_Update();

	if (WAV_Dumping) Update_WAV_Dump();
//	if (GYM_Dumping) Update_GYM_Dump((unsigned char) 0, (unsigned char) 0, (unsigned char) 0);

	return(1);
}


int Do_Genesis_Frame()
{
	int *buf[2];
	int HInt_Counter;

	if ((CPU_Mode) && (VDP_Reg.Set2 & 0x8))	VDP_Num_Vis_Lines = 240;
	else VDP_Num_Vis_Lines = 224;

	YM_Buf[0] = PSG_Buf[0] = Seg_L;
	YM_Buf[1] = PSG_Buf[1] = Seg_R;
	YM_Len = PSG_Len = 0;

	Cycles_M68K = Cycles_Z80 = 0;
	Last_BUS_REQ_Cnt = -1000;
	main68k_tripOdometer();
	z80_Clear_Odo(&M_Z80);

//	Patch_Codes();

	VRam_Flag = 1;

	VDP_Status &= 0xFFF7;							// Clear V Blank
	if (VDP_Reg.Set4 & 0x2) VDP_Status ^= 0x0010;

	HInt_Counter = VDP_Reg.H_Int;					// Hint_Counter = step d'interruption H

	for(VDP_Current_Line = 0; VDP_Current_Line < VDP_Num_Vis_Lines; VDP_Current_Line++)
	{
		buf[0] = Seg_L + Sound_Extrapol[VDP_Current_Line][0];
		buf[1] = Seg_R + Sound_Extrapol[VDP_Current_Line][0];
		YM2612_DacAndTimers_Update(buf, Sound_Extrapol[VDP_Current_Line][1]);
		YM_Len += Sound_Extrapol[VDP_Current_Line][1];
		PSG_Len += Sound_Extrapol[VDP_Current_Line][1];

		Fix_Controllers();
		Cycles_M68K += CPL_M68K;
		Cycles_Z80 += CPL_Z80;
		if (DMAT_Lenght) main68k_addCycles(Update_DMA());

		VDP_Status |= 0x0004;			// HBlank = 1
		main68k_exec(Cycles_M68K - 404);
		VDP_Status &= 0xFFFB;			// HBlank = 0

		if (--HInt_Counter < 0)
		{
			HInt_Counter = VDP_Reg.H_Int;
			VDP_Int |= 0x4;
			Update_IRQ_Line();
		}

		Render_Line();

		main68k_exec(Cycles_M68K);
		if (Z80_State == 3) z80_Exec(&M_Z80, Cycles_Z80);
		else z80_Set_Odo(&M_Z80, Cycles_Z80);
	}

	buf[0] = Seg_L + Sound_Extrapol[VDP_Current_Line][0];
	buf[1] = Seg_R + Sound_Extrapol[VDP_Current_Line][0];
	YM2612_DacAndTimers_Update(buf, Sound_Extrapol[VDP_Current_Line][1]);
	YM_Len += Sound_Extrapol[VDP_Current_Line][1];
	PSG_Len += Sound_Extrapol[VDP_Current_Line][1];

	Fix_Controllers();
	Cycles_M68K += CPL_M68K;
	Cycles_Z80 += CPL_Z80;
	if (DMAT_Lenght) main68k_addCycles(Update_DMA());

	if (--HInt_Counter < 0)
	{
		VDP_Int |= 0x4;
		Update_IRQ_Line();
	}

	VDP_Status |= 0x000C;			// VBlank = 1 et HBlank = 1 (retour de balayage vertical en cours)
	main68k_exec(Cycles_M68K - 360);
	if (Z80_State == 3) z80_Exec(&M_Z80, Cycles_Z80 - 168);
	else z80_Set_Odo(&M_Z80, Cycles_Z80 - 168);

	VDP_Status &= 0xFFFB;			// HBlank = 0
	VDP_Status |= 0x0080;			// V Int happened

	VDP_Int |= 0x8;
	Update_IRQ_Line();
	z80_Interrupt(&M_Z80, 0xFF);

	main68k_exec(Cycles_M68K);
	if (Z80_State == 3) z80_Exec(&M_Z80, Cycles_Z80);
	else z80_Set_Odo(&M_Z80, Cycles_Z80);

	for(VDP_Current_Line++; VDP_Current_Line < VDP_Num_Lines; VDP_Current_Line++)
	{
		buf[0] = Seg_L + Sound_Extrapol[VDP_Current_Line][0];
		buf[1] = Seg_R + Sound_Extrapol[VDP_Current_Line][0];
		YM2612_DacAndTimers_Update(buf, Sound_Extrapol[VDP_Current_Line][1]);
		YM_Len += Sound_Extrapol[VDP_Current_Line][1];
		PSG_Len += Sound_Extrapol[VDP_Current_Line][1];

		Fix_Controllers();
		Cycles_M68K += CPL_M68K;
		Cycles_Z80 += CPL_Z80;
		if (DMAT_Lenght) main68k_addCycles(Update_DMA());

		VDP_Status |= 0x0004;					// HBlank = 1
//		main68k_exec(Cycles_M68K - 436);
		main68k_exec(Cycles_M68K - 404);
		VDP_Status &= 0xFFFB;					// HBlank = 0

		main68k_exec(Cycles_M68K);
		if (Z80_State == 3) z80_Exec(&M_Z80, Cycles_Z80);
		else z80_Set_Odo(&M_Z80, Cycles_Z80);
	}

	PSG_Special_Update();
	YM2612_Special_Update();

	if (WAV_Dumping) Update_WAV_Dump();
//	if (GYM_Dumping) Update_GYM_Dump((unsigned char) 0, (unsigned char) 0, (unsigned char) 0);

	return(1);
}


