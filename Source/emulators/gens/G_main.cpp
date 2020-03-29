#include "gens.h"
#include <xtl.h>

#include <stdio.h>
#include <string.h>
#include "G_main.h"
#include "G_ddraw.h"
#include "G_dsound.h"
#include "G_Input.h"
#include "rom.h"
#include "save.h"

#include "misc.h"
#include "blit.h"
#include "Cpu_68k.h"
#include "Star_68k.h"
#include "Cpu_Z80.h"
#include "z80.h"
#include "mem_M68K.h"
#include "mem_Z80.h"
#include "io.h"
#include "psg.h"
#include "ym2612.h"
#include "vdp_io.h"
#include "vdp_rend.h"
#include "pcm.h"


HWND HWnd;
extern "C" void Read_To_68K_Space(int adr);

extern "C" void xbox_set_refreshrate( int rate ) ;

#define WM_KNUX WM_USER + 3
#define GENS_VERSION   2.99
#define GENS_VERSION_H 2 * 65536 + 10

#define MINIMIZE								\
if (Sound_Initialised) Clear_Sound_Buffer();	\
if (Full_Screen)								\
{												\
	Set_Render(hWnd, 0, -1, true);				\
	FS_Minimised = 1;							\
}

#define MENU_L(smenu, pos, flags, id, str, suffixe, def)										\
GetPrivateProfileString(language_name[Language], (str), (def), Str_Tmp, 1024, Language_Path);	\
strcat(Str_Tmp, (suffixe));																			\
InsertMenu((smenu), (pos), (flags), (id), Str_Tmp);

#define WORD_L(id, str, suffixe, def)															\
GetPrivateProfileString(language_name[Language], (str), (def), Str_Tmp, 1024, Language_Path);	\
strcat(Str_Tmp, (suffixe));																			\
SetDlgItemText(hDlg, id, Str_Tmp);

#define MESSAGE_L(str, def, time)																	\
{																									\
														\
}



int GYM_Dumping = 0;


char Str_Tmp[1024];
char Gens_Path[1024];

char **language_name = NULL;
struct Rom *Game = NULL;
int Active = 0;
int Paused = 0;
int Net_Play = 0;
int Full_Screen = -1;
int Resolution = 1;
int Fast_Blur = 0;
int Render_W = 0;
int Render_FS = 0;
int Show_FPS = 0;
int Show_Message = 0;
int Show_LED = 0;
int FS_Minimised = 0;
int Auto_Pause = 0;
int Auto_Fix_CS = 0;
int Language = 0;
int Country = -1;
int Country_Order[3];
int Kaillera_Client_Running = 0;
int Intro_Style = 0;
int SegaCD_Accurate = 0;
int Gens_Running = 0;
int WinNT_Flag = 0;
int Gens_Priority;
int SS_Actived;

int Set_Render(HWND hWnd, int Num, bool Force);

int Set_Render(HWND hWnd, int Full, int Num, int Force)
{
	int Old_Rend, *Rend;
	void (**Blit)(unsigned char*, int, int, int, int);
	
	if (Full)
	{
		Rend = &Render_FS;
		Blit = &Blit_FS;
	}
	else
	{
		Rend = &Render_W;
		Blit = &Blit_W;
	}

	Old_Rend = *Rend;
	Flag_Clr_Scr = 1;

	switch(Num)
	{
		case -1:
			switch(Old_Rend)
			{
				case 0:
					if (Have_MMX) *Blit = Blit_X1_MMX;
					else *Blit = Blit_X1;
					break;

				case 1:
					if (Have_MMX) *Blit = Blit_X2_MMX;
					else *Blit = Blit_X2;
					break;

				case 2:
					if (Have_MMX) *Blit = Blit_X2_Int_MMX;
					else *Blit =  Blit_X2_Int;
					break;

				case 3:
					if (Have_MMX) *Blit = Blit_Scanline_MMX;
					else *Blit = Blit_Scanline;
					break;

				case 4:
					if (Have_MMX) *Blit = Blit_Scanline_50_MMX;
					else
					{
						*Rend = 1;
						*Blit = Blit_X2;
					}
					break;

				case 5:
					if (Have_MMX) *Blit = Blit_Scanline_25_MMX;
					else
					{
						*Rend = 1;
						*Blit = Blit_X2;
					}
					break;

				case 6:
					if (Have_MMX) *Blit = Blit_Scanline_Int_MMX;
					else *Blit = Blit_Scanline_Int;
					break;

				case 7:
					if (Have_MMX) *Blit = Blit_Scanline_50_Int_MMX;
					else
					{
						*Rend = 1;
						*Blit = Blit_X2;
					}
					break;

				case 8:
					if (Have_MMX) *Blit = Blit_Scanline_25_Int_MMX;
					else
					{
						*Rend = 1;
						*Blit = Blit_X2;
					}
					break;

				case 9:
					if (Have_MMX) *Blit = Blit_2xSAI_MMX;
					else
					{
						*Rend = 1;
						*Blit = Blit_X2;
					}
					break;

				default:
					*Rend = 1;
					if (Have_MMX) *Blit = Blit_X2_MMX;
					else *Blit = Blit_X2;
					break;
			}
			break;

		case 0:
			*Rend = 0;
			if (Have_MMX) *Blit = Blit_X1_MMX;
			else *Blit = Blit_X1;
			break;

		case 1:
			*Rend = 1;
			if (Have_MMX) *Blit = Blit_X2_MMX;
			else *Blit = Blit_X2;
			break;

		case 2:
			*Rend = 2;
			if (Have_MMX) *Blit = Blit_X2_Int_MMX;
			else *Blit = Blit_X2_Int;
			break;

		case 3:
			*Rend = 3;
			if (Have_MMX) *Blit = Blit_Scanline_MMX;
			else *Blit = Blit_Scanline;
			break;

		case 4:
			if (Have_MMX)
			{
				*Rend = 4;
				*Blit = Blit_Scanline_50_MMX;
			}
			else
			{
				*Rend = 6;
				*Blit = Blit_Scanline_Int;
			}
			break;

		case 5:
			if (Have_MMX)
			{
				*Rend = 5;
				*Blit = Blit_Scanline_25_MMX;
			}
			else
			{
				*Rend = 3;
				*Blit = Blit_Scanline;
			}
			break;

		case 6:
			*Rend = 6;
			if (Have_MMX) *Blit = Blit_Scanline_Int_MMX;
			else *Blit = Blit_Scanline_Int;
			break;

		case 7:
			if (Have_MMX)
			{
				*Rend = 7;
				*Blit = Blit_Scanline_50_Int_MMX;
			}
			else
			{
				*Rend = 6;
				*Blit = Blit_Scanline_Int;
			}
			break;

		case 8:
			if (Have_MMX)
			{
				*Rend = 8;
				*Blit = Blit_Scanline_25_Int_MMX;
			}
			else
			{
				*Rend = 6;
				*Blit = Blit_Scanline_Int;
				}
			break;

		case 9:
			if (Have_MMX)
			{
				*Rend = 9;
				*Blit = Blit_2xSAI_MMX;
			}
			else
			{
				*Rend = 6;
				*Blit = Blit_Scanline_Int;
			}
			break;

		default:
			*Rend = 1;
			if (Have_MMX) *Blit = Blit_X2_MMX;
			else *Blit = Blit_X2;
			break;
	}

	if (!((Full == Full_Screen) && ((Num >=2) && (Old_Rend >= 2)) && (!Force)))
	{
		if (Sound_Initialised) 
			Clear_Sound_Buffer();
		return 1 ;
	}

	return(1);
}



int Change_Z80(HWND hWnd)
{
	if (Z80_State & 1)
	{
		Z80_State &= ~1;
	}
	else
	{
		Z80_State |= 1;
	}
	return(1);
}


int Change_DAC(HWND hWnd)
{
	if (DAC_Enable)
	{
		DAC_Enable = 0;
		MESSAGE_L("DAC Disabled", "DAC Disabled", 1000)
	}
	else
	{
		DAC_Enable = 1;
		MESSAGE_L("DAC Enabled", "DAC Enabled", 1000)
	}

	return(1);
}


int Change_DAC_Improv(HWND hWnd)
{
	if (DAC_Improv)
	{
		DAC_Improv = 0;
		MESSAGE_L("Normal DAC sound", "Normal DAC sound", 1000)
	}
	else
	{
		DAC_Improv = 1;
		MESSAGE_L("Improved DAC sound (voices)", "Improved DAC sound (voices)", 1000)
	}

	return(1);
}


int Change_YM2612(HWND hWnd)
{
	if (YM2612_Enable)
	{
		YM2612_Enable = 0;
		MESSAGE_L("YM2612 Disabled", "YM2612 Disabled", 1000)
	}
	else
	{
		YM2612_Enable = 1;
		MESSAGE_L("YM2612 Enabled", "YM2612 Enabled", 1000)
	}

	return(1);
}


int Change_YM2612_Improv(HWND hWnd)
{
	unsigned char Reg_1[0x200];

	if (YM2612_Improv)
	{
		YM2612_Improv = 0;
		MESSAGE_L("Normal YM2612 emulation", "Normal YM2612 emulation", 1000)
	}
	else
	{
		YM2612_Improv = 1;
		MESSAGE_L("High Quality YM2612 emulation", "High Quality YM2612 emulation", 1000)
	}

	YM2612_Save(Reg_1);

	if (CPU_Mode)
	{
		YM2612_Init(CLOCK_PAL / 7, Sound_Rate, YM2612_Improv);
	}
	else
	{
		YM2612_Init(CLOCK_NTSC / 7, Sound_Rate, YM2612_Improv);
	}

	YM2612_Restore(Reg_1);

	return 1;
}


int Change_PSG(HWND hWnd)
{
	if (PSG_Enable)
	{
		PSG_Enable = 0;
		MESSAGE_L("PSG Disabled", "PSG Disabled", 1000)
	}
	else
	{
		PSG_Enable = 1;
		MESSAGE_L("PSG Enabled", "PSG Enabled", 1000)
	}

	return 1;
}


int Change_PSG_Improv(HWND hWnd)
{
	if (PSG_Improv)
	{
		PSG_Improv = 0;
		MESSAGE_L("Normal PSG sound", "Normal PSG sound", 1000)
	}
	else
	{
		PSG_Improv = 1;
		MESSAGE_L("Improved PSG sound", "Improved PSG sound", 1000)
	}

	return 1;
}


int Change_PCM(HWND hWnd)
{
	if (PCM_Enable)
	{
		PCM_Enable = 0;
		MESSAGE_L("PCM Sound Disabled", "PCM Sound Disabled", 1000)
	}
	else
	{
		PCM_Enable = 1;
		MESSAGE_L("PCM Sound Enabled", "PCM Sound Enabled", 1000)
	}

	return 1;
}

int	Change_Sound(HWND hWnd)
{
	if (Sound_Enable)
	{
		End_Sound();

		Sound_Enable = 0;
		YM2612_Enable = 0;
		PSG_Enable = 0;
		DAC_Enable = 0;
		PCM_Enable = 0;

		MESSAGE_L("Sound Disabled", "Sound Disabled", 1500)
	}
	else
	{
		if (!Init_Sound(hWnd))
		{
			Sound_Enable = 0;
			YM2612_Enable = 0;
			PSG_Enable = 0;
			DAC_Enable = 0;
			PCM_Enable = 0;
			return 0;
		}

		Sound_Enable = 1;
		Play_Sound();

		if (!(Z80_State & 1)) 
			Change_Z80(hWnd);

		YM2612_Enable = 1;
		PSG_Enable = 1;
		DAC_Enable = 1;
		PCM_Enable = 1;
		MESSAGE_L("Sound Enabled", "Sound Enabled", 1500)
	}

	return 1;
}


int Change_Sample_Rate(HWND hWnd, int Rate)
{
	unsigned char Reg_1[0x200];

	switch (Rate)
	{
	case 0:
		Sound_Rate = 11025;
		MESSAGE_L("Sound rate set to 11025", "Sound rate set to 11025", 2500)
		break;

	case 1:
		Sound_Rate = 22050;
		MESSAGE_L("Sound rate set to 22050", "Sound rate set to 22050", 2500)
		break;

	case 2:
		Sound_Rate = 44100;
		MESSAGE_L("Sound rate set to 44100", "Sound rate set to 44100", 2500)
		break;
	}

	if (Sound_Enable)
	{
		PSG_Save_State();
		YM2612_Save(Reg_1);

		End_Sound();
		Sound_Enable = 0;

		if (CPU_Mode)
		{
			YM2612_Init(CLOCK_PAL / 7, Sound_Rate, YM2612_Improv);
			PSG_Init(CLOCK_PAL / 15, Sound_Rate);
		}
		else
		{
			YM2612_Init(CLOCK_NTSC / 7, Sound_Rate, YM2612_Improv);
			PSG_Init(CLOCK_NTSC / 15, Sound_Rate);
		}

		if (SegaCD_Started) Set_Rate_PCM(Sound_Rate);
		YM2612_Restore(Reg_1);
		PSG_Restore_State();
		
		if(!Init_Sound(hWnd)) return(0);

		Sound_Enable = 1;
		Play_Sound();
	}
	return(1);
}


int Change_Sound_Stereo(HWND hWnd)
{
	unsigned char Reg_1[0x200];

	if (Sound_Stereo)
	{
		Sound_Stereo = 0;
		MESSAGE_L("Mono sound", "Mono sound", 1000)
	}
	else
	{
		Sound_Stereo = 1;
		MESSAGE_L("Stereo sound", "Stereo sound", 1000)
	}

	if (Sound_Enable)
	{
		PSG_Save_State();
		YM2612_Save(Reg_1);

		End_Sound();
		Sound_Enable = 0;

		if (CPU_Mode)
		{
			YM2612_Init(CLOCK_PAL / 7, Sound_Rate, YM2612_Improv);
			PSG_Init(CLOCK_PAL / 15, Sound_Rate);
		}
		else
		{
			YM2612_Init(CLOCK_NTSC / 7, Sound_Rate, YM2612_Improv);
			PSG_Init(CLOCK_NTSC / 15, Sound_Rate);
		}

		if (SegaCD_Started) Set_Rate_PCM(Sound_Rate);
		YM2612_Restore(Reg_1);
		PSG_Restore_State();
		
		if(!Init_Sound(hWnd)) return(0);

		Sound_Enable = 1;
		Play_Sound();
	}
	return(1);
}


int Change_Country(HWND hWnd, int Num)
{
	unsigned char Reg_1[0x200];

	Flag_Clr_Scr = 1;

	switch(Country = Num)
	{
		default:
		case -1:
			if (Genesis_Started || _32X_Started) 
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

	if (CPU_Mode)
	{
		CPL_Z80 = Round_Double((((double) CLOCK_PAL / 15.0) / 50.0) / 312.0);
		CPL_M68K = Round_Double((((double) CLOCK_PAL / 7.0) / 50.0) / 312.0);

		VDP_Num_Lines = 312;
		VDP_Status |= 0x0001;

	}
	else
	{
		CPL_Z80 = Round_Double((((double) CLOCK_NTSC / 15.0) / 60.0) / 262.0);
		CPL_M68K = Round_Double((((double) CLOCK_NTSC / 7.0) / 60.0) / 262.0);

		VDP_Num_Lines = 262;
		VDP_Status &= 0xFFFE;
	}

	if (Sound_Enable)
	{
		PSG_Save_State();
		YM2612_Save(Reg_1);

		End_Sound();
		Sound_Enable = 0;

		if (CPU_Mode)
		{
			YM2612_Init(CLOCK_PAL / 7, Sound_Rate, YM2612_Improv);
			PSG_Init(CLOCK_PAL / 15, Sound_Rate);
		}
		else
		{
			YM2612_Init(CLOCK_NTSC / 7, Sound_Rate, YM2612_Improv);
			PSG_Init(CLOCK_NTSC / 15, Sound_Rate);
		}

		YM2612_Restore(Reg_1);
		PSG_Restore_State();
		
		if(!Init_Sound(hWnd)) return(0);

		Sound_Enable = 1;
		Play_Sound();
	}

	if (Game_Mode)
	{
		if (CPU_Mode) MESSAGE_L("Europe system (50 FPS)", "Europe system (50 FPS)", 1500)
		else MESSAGE_L("USA system (60 FPS)", "USA system (60 FPS)", 1500)
	}
	else
	{
		if (CPU_Mode) MESSAGE_L("Japan system (50 FPS)", "Japan system (50 FPS)", 1500)
		else MESSAGE_L("Japan system (60 FPS)", "Japan system (60 FPS)", 1500)
	}

	if (Genesis_Started)
	{
		if ((CPU_Mode == 1) || (Game_Mode == 0))
			sprintf(Str_Tmp, "Gens - Megadrive : %s", Game->Rom_Name_W);
		else
			sprintf(Str_Tmp, "Gens - Genesis : %s", Game->Rom_Name_W);
	}
	return 1;
}


int Change_Country_Order(int Num)
{
	char c_str[4][4] = {"USA", "JAP", "EUR"};
	char str_w[128];
	int sav = Country_Order[Num];
		
	if (Num == 1) Country_Order[1] = Country_Order[0];
	else if (Num == 2)
	{
		Country_Order[2] = Country_Order[1];
		Country_Order[1] = Country_Order[0];
	}
	Country_Order[0] = sav;

	if (Country == -1) Change_Country(HWnd, -1);		// Update Country

	wsprintf(str_w, "Country detec.order : %s %s %s", c_str[Country_Order[0]], c_str[Country_Order[1]], c_str[Country_Order[2]]);
	MESSAGE_L(str_w, str_w, 1500)
	return(1);
}

BOOL Init(HINSTANCE hInst, int nCmdShow)
{
	Net_Play = 0;
	Full_Screen = 1;
	VDP_Num_Vis_Lines = 224;
	Resolution = 1;
	W_VSync = 0;
	FS_VSync = 0;
	Stretch = 0;
	Sprite_Over = 1;
	Render_W = 0;
	Render_FS = 0;
	Show_Message = 1;

	Sound_Enable = 0;
	Sound_Segs = 8;
	Sound_Stereo = 1;
	Sound_Initialised = 0;
	Sound_Is_Playing = 0;
	WAV_Dumping = 0;
	GYM_Dumping = 0;

	FS_Minimised = 0;
	Game = NULL;
	Genesis_Started = 0;
	SegaCD_Started = 0;
	_32X_Started = 0;
	Debug = 0;
	CPU_Mode = 0;
	Identify_CPU();

	WinNT_Flag = 1 ;
	strcpy( Gens_Path, "D:\\SGENROMS" ) ;
	strcpy( Str_Tmp, "D:\\SGENROMS" ) ;
	strcat(Gens_Path, "\\");
	strcat(Str_Tmp, "\\gens.cfg");
	M68K_Init();
	Z80_Init();
	YM2612_Init(CLOCK_NTSC / 7, Sound_Rate, YM2612_Improv);
	PSG_Init(CLOCK_NTSC / 15, Sound_Rate);
	Load_Config(Str_Tmp, NULL);
	if (!Init_Input(hInst, HWnd))
	{
		End_Sound();
		End_DDraw();
		return FALSE;
	}
	Init_Tab();
	Gens_Running = 1;

	return TRUE;
}


void End_All(void)
{
	Free_Rom(Game);
	End_DDraw();
	End_Input();
	YM2612_End();
	End_Sound();
}



int GensInit(char *romfilename, int country)
{
	throttle = 0 ;
	Init(0, 0);
	Country = country ;
	// Have to do it *before* load by command line
	Init_Genesis_Bios();
	Pre_Load_Rom(0,romfilename) ;
	Active = 1 ;
	return 0;
}

void GensFrame()
{
	Update_Emulation(HWnd);
	//Update_Emulation_One(HWnd);
	return;
}

int GensRender()
{
	return 0;
}

int GensExit()
{
	strcpy(Str_Tmp, Gens_Path);
	strcat(Str_Tmp, "Gens.cfg");
	End_All();
	return 0;
}


int PASCAL gensmain( char *romfilename, int country, int whichCD, int realCD )
{
	Init(0, 0);
	Country = country ;
	// Have to do it *before* load by command line
	Init_Genesis_Bios();
	Pre_Load_Rom(0,romfilename) ;
	Active = 1 ;
	while (Gens_Running)
	{

		if (Genesis_Started)
		{
			if ((Active) && (!Paused))	// EMULATION
			{
				Update_Emulation(HWnd);
			}
			else						// EMULATION PAUSED
			{
				Flip(HWnd);
				Sleep(100);
			}
		}
	}
	strcpy(Str_Tmp, Gens_Path);
	strcat(Str_Tmp, "Gens.cfg");
	End_All();
	return 0;
}

