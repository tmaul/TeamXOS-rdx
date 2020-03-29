#include <stdio.h>
#include <math.h>
#include "G_ddraw.h"
#include "G_dsound.h"
#include "G_Input.h"
#include "G_main.h"
#include "gens.h"
#include "mem_M68K.h"
#include "vdp_io.h"
#include "vdp_rend.h"
#include "misc.h"
#include "blit.h"

clock_t Last_Time = 0, New_Time = 0;
clock_t Used_Time = 0;

int Flag_Clr_Scr = 0;
int Sleep_Time;
int FS_VSync;
int W_VSync;
int Stretch; 
int Blit_Soft;
int Effect_Color = 7;
int FPS_Style = EMU_MODE | BLANC;
int Message_Style = EMU_MODE | BLANC | SIZE_X2;
int Kaillera_Error = 0;
int droppedFrames = 0 ;
int totalFrames = 0 ;
static char Info_String[1024] = "";
static int Message_Showed = 0;
static unsigned int Info_Time = 0;
extern int Gens_Running ;


#define MB_OK 1

void (*Blit_FS)(unsigned char *Dest, int pitch, int x, int y, int offset);
void (*Blit_W)(unsigned char *Dest, int pitch, int x, int y, int offset);
int (*Update_Frame)();
int (*Update_Frame_Fast)();


void Put_Info(char *Message, int Duree)
{
	if (Show_Message)
	{
		strcpy(Info_String, Message);
		Info_Time = GetTickCount() + Duree;
		Message_Showed = 1;
	}
}


int Init_Fail(HWND hwnd, char *err)
{
	return 0;
}


int Init_DDraw(HWND hWnd)
{
	droppedFrames = 0 ;
	totalFrames = 0 ;
	return 1;
}

void End_DDraw()
{
}

int Flip(HWND hWnd)
{
	int i;
	static float FPS = 0.0f, frames[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	static unsigned int old_time = 0, view_fps = 0, index_fps = 0, freq_cpu[2] = {0, 0};
	unsigned int new_time[2];

	if ( hWnd )
	{
		freq_cpu[0] = freq_cpu[1] = 0 ;
		old_time = 0 ;
	}

	{	
		if (freq_cpu[0] > 1)				// accurate timer ok
		{
			if (++view_fps >= 16)
			{
				QueryPerformanceCounter((union _LARGE_INTEGER *) new_time);

				if (new_time[0] != old_time)
				{					
					FPS = (float) (freq_cpu[0]) * 16.0f / (float) (new_time[0] - old_time);
				}
				else
				{
				}
				old_time = new_time[0];
				view_fps = 0;
			}
		}
		else if (freq_cpu[0] == 1)			// accurate timer not supported
		{
			if (++view_fps >= 10)
			{
				new_time[0] = GetTickCount();
		
				if (new_time[0] != old_time) frames[index_fps] = 10000 / (float)(new_time[0] - old_time);
				else frames[index_fps] = 2000;

				index_fps++;
				index_fps &= 7;
				FPS = 0.0f;

				for(i = 0; i < 8; i++) FPS += frames[i];

				FPS /= 8.0f;
				old_time = new_time[0];
				view_fps = 0;
			}
		}
		else
		{
			QueryPerformanceFrequency((union _LARGE_INTEGER *) freq_cpu);
			if (freq_cpu[0] == 0) freq_cpu[0] = 1;
		}

	}

	return 1;
}

int throttle =0;



int Update_Emulation(HWND hWnd)
{
	xbox_check_events() ;
	{
		Update_Frame();
		//xbox_put_image() ;
		xbox_process_audio() ;
	}
	return 1;
}


int Update_Emulation_One(HWND hWnd)
{
	Update_Controllers();
	Update_Frame();
	Flip(hWnd);

	return 1;
}

int Eff_Screen(void)
{
	int i;

	for(i = 0; i < 336 * 240; i++) 
		MD_Screen[i] = 0;

	return 1;
}


int Pause_Screen(void)
{
	int i, j, offset;
	int r, v, b, nr, nv, nb;

	r = v = b = nr = nv = nb = 0;

	for(offset = j = 0; j < 240; j++)
	{
		for(i = 0; i < 336; i++, offset++)
		{
			if (Mode_555 & 1)
			{
				r = (MD_Screen[offset] & 0x7C00) >> 10;
				v = (MD_Screen[offset] & 0x03E0) >> 5;
				b = (MD_Screen[offset] & 0x001F);
			}
			else
			{
				r = (MD_Screen[offset] & 0xF800) >> 11;
				v = (MD_Screen[offset] & 0x07C0) >> 6;
				b = (MD_Screen[offset] & 0x001F);
			}

			nr = nv = nb = (r + v + b) / 3;
			
			if ((nb <<= 1) > 31) nb = 31;

			nr &= 0x1E;
			nv &= 0x1E;
			nb &= 0x1E;

			if (Mode_555 & 1)
				MD_Screen[offset] = (nr << 10) + (nv << 5) + nb;
			else
				MD_Screen[offset] = (nr << 11) + (nv << 6) + nb;
		}
	}

	return 1;
}


int Show_Genesis_Screen(HWND hWnd)
{
	Do_VDP_Only();
	Flip(hWnd);

	return 1;
}

