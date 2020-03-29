#include <stdio.h>
#include "G_ddraw.h"
#include "G_dsound.h"
#include "psg.h"
#include "ym2612.h"
#include "mem_M68K.h"
#include "vdp_io.h"
#include "G_Main.h"
#include "gens.h"
#include "Rom.h"
//#include "wave.h"
#include "pcm.h"
#include "misc.h"		// for Have_MMX flag

void End_Sound(void);

int Seg_L[882], Seg_R[882];
int Seg_Lenght, SBuffer_Lenght;
int Sound_Rate = 22050, Sound_Segs = 8;
int Bytes_Per_Unit;
int Sound_Enable;
int Sound_Stereo = 1;
int Sound_Is_Playing = 0;
int Sound_Initialised = 0;
int WAV_Dumping = 0;
int GYM_Playing = 0;
int WP, RP;

unsigned int Sound_Interpol[882];
unsigned int Sound_Extrapol[312][2];

int Init_Sound(HWND hWnd)
{
	//WAVEFORMATEX wfx;
	int i;
 
	if (Sound_Initialised) return 0;
	End_Sound();
	
	switch (Sound_Rate)
	{
		case 11025:
			if (CPU_Mode)
				Seg_Lenght = 220;
			else
				Seg_Lenght = 184;
			break;
			
		case 22050:
			if (CPU_Mode)
				Seg_Lenght = 441;
			else
				Seg_Lenght = 368;
			break;
			
		case 44100:
			if (CPU_Mode)
				Seg_Lenght = 882;
			else
				Seg_Lenght = 735;
			break;
	}

	if (CPU_Mode)
	{
		for(i = 0; i < 312; i++)
		{
			Sound_Extrapol[i][0] = ((Seg_Lenght * i) / 312);
			Sound_Extrapol[i][1] = (((Seg_Lenght * (i + 1)) / 312) - Sound_Extrapol[i][0]);
		}

		for(i = 0; i < Seg_Lenght; i++)
			Sound_Interpol[i] = ((312 * i) / Seg_Lenght);
	}
	else
	{
		for(i = 0; i < 262; i++)
		{
			Sound_Extrapol[i][0] = ((Seg_Lenght * i) / 262);
			Sound_Extrapol[i][1] = (((Seg_Lenght * (i + 1)) / 262) - Sound_Extrapol[i][0]);
		}

		for(i = 0; i < Seg_Lenght; i++)
			Sound_Interpol[i] = ((262 * i) / Seg_Lenght);
	}

	memset(Seg_L, 0, Seg_Lenght << 2);
	memset(Seg_R, 0, Seg_Lenght << 2);

	WP = 0;
	RP = 0;

	//rval = DirectSoundCreate(NULL, &lpDS, NULL);

	//if (rval != DS_OK) return 0;

	//rval = lpDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
//	rval = lpDS->SetCooperativeLevel(hWnd, DSSCL_WRITEPRIMARY);

	//if(rval != DS_OK) 
	//{
		//lpDS->Release();
		//lpDS = NULL;
		//return 0;
	//}

	//memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
    //dsbdesc.dwSize = sizeof(DSBUFFERDESC);
    //dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

	/*
	rval = lpDS->CreateSoundBuffer(&dsbdesc, &lpDSPrimary, NULL);

	if(rval != DS_OK) 
	{
		lpDS->Release();
		lpDS = NULL;
		return 0;
	}

    memset(&wfx, 0, sizeof(WAVEFORMATEX));
    wfx.wFormatTag = WAVE_FORMAT_PCM; 
	if (Sound_Stereo) wfx.nChannels = 2;
	else wfx.nChannels = 1;
	wfx.nSamplesPerSec = Sound_Rate;
	wfx.wBitsPerSample = 16;
	wfx.nBlockAlign = Bytes_Per_Unit = (wfx.wBitsPerSample / 8) * wfx.nChannels;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * Bytes_Per_Unit;

	rval = lpDSPrimary->SetFormat(&wfx);
 
	if(rval != DS_OK) 
	{
		lpDSPrimary->Release();
		lpDSPrimary = NULL;
		lpDS->Release();
		lpDS = NULL;
		return 0;
	}

    memset(&MainWfx, 0, sizeof(WAVEFORMATEX));
    MainWfx.wFormatTag = WAVE_FORMAT_PCM; 
	if (Sound_Stereo) MainWfx.nChannels = 2;
	else MainWfx.nChannels = 1;
	MainWfx.nSamplesPerSec = Sound_Rate;
	MainWfx.wBitsPerSample = 16;
	MainWfx.nBlockAlign = Bytes_Per_Unit = (MainWfx.wBitsPerSample / 8) * MainWfx.nChannels;
	MainWfx.nAvgBytesPerSec = MainWfx.nSamplesPerSec * Bytes_Per_Unit;

	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 
    dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
	dsbdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
	dsbdesc.dwBufferBytes = SBuffer_Lenght = Seg_Lenght * Sound_Segs * Bytes_Per_Unit;
	dsbdesc.lpwfxFormat = &MainWfx;

//	sprintf(STR, "Seg l : %d   Num Seg : %d   Taille : %d", Seg_Lenght, Sound_Segs, Bytes_Per_Unit);
//	MessageBox(HWnd, STR, "", MB_OK);

	rval = lpDS->CreateSoundBuffer(&dsbdesc, &lpDSBuffer, NULL);

	if(rval != DS_OK) 
	{
		lpDS->Release();
		lpDS = NULL;
		return 0;
	}
*/
	return(Sound_Initialised = 1);
	
//return 1 ;
}


void End_Sound()
{
	
	if(Sound_Initialised)
	{
		/*
		if (lpDSPrimary)
		{
			lpDSPrimary->Release();
			lpDSPrimary = NULL;
		}

		if (lpDSBuffer)
		{
			lpDSBuffer->Stop();
			Sound_Is_Playing = 0;
			lpDSBuffer->Release();
			lpDSBuffer = NULL;
		}

		if (lpDS)
		{
			lpDS->Release();
			lpDS = NULL;
		}
*/
		Sound_Initialised = 0;
	}
	
}

int Get_Current_Seg(void)
{
	/*
	unsigned long R;
	
	lpDSBuffer->GetCurrentPosition(&R, NULL);
	return(R / (Seg_Lenght * Bytes_Per_Unit));
	*/
	return 0 ;
}


int Check_Sound_Timing(void)
{
	/*
	unsigned long R;
	
	lpDSBuffer->GetCurrentPosition(&R, NULL);

	RP = R / (Seg_Lenght * Bytes_Per_Unit);

	if (RP == ((WP + 1) & (Sound_Segs - 1))) return 2;

	if (RP != WP) return 1;
*/
	return 0;
}


void Write_Sound_Stereo(short *Dest, int lenght)
{
	int i, out_L, out_R;
	short *dest = Dest;
	
	for(i = 0; i < Seg_Lenght; i++)
	{
		out_L = Seg_L[i];
		Seg_L[i] = 0;

		if (out_L < -0x7FFF) *dest++ = -0x7FFF;
		else if (out_L > 0x7FFF) *dest++ = 0x7FFF;
		else *dest++ = (short) out_L;
						
		out_R = Seg_R[i];
		Seg_R[i] = 0;

		if (out_R < -0x7FFF) *dest++ = -0x7FFF;
		else if (out_R > 0x7FFF) *dest++ = 0x7FFF;
		else *dest++ = (short) out_R;
	}
}


void Dump_Sound_Stereo(short *Dest, int lenght)
{
	int i, out_L, out_R;
	short *dest = Dest;
	
	for(i = 0; i < Seg_Lenght; i++)
	{
		out_L = Seg_L[i];

		if (out_L < -0x7FFF) *dest++ = -0x7FFF;
		else if (out_L > 0x7FFF) *dest++ = 0x7FFF;
		else *dest++ = (short) out_L;
						
		out_R = Seg_R[i];

		if (out_R < -0x7FFF) *dest++ = -0x7FFF;
		else if (out_R > 0x7FFF) *dest++ = 0x7FFF;
		else *dest++ = (short) out_R;
	}
}


void Write_Sound_Mono(short *Dest, int lenght)
{
	int i, out;
	short *dest = Dest;
	
	for(i = 0; i < Seg_Lenght; i++)
	{
		out = Seg_L[i] + Seg_R[i];
		Seg_L[i] = Seg_R[i] = 0;

		if (out < -0xFFFF) *dest++ = -0x7FFF;
		else if (out > 0xFFFF) *dest++ = 0x7FFF;
		else *dest++ = (short) (out >> 1);
	}
}


void Dump_Sound_Mono(short *Dest, int lenght)
{
	int i, out;
	short *dest = Dest;
	
	for(i = 0; i < Seg_Lenght; i++)
	{
		out = Seg_L[i] + Seg_R[i];

		if (out < -0xFFFF) *dest++ = -0x7FFF;
		else if (out > 0xFFFF) *dest++ = 0x7FFF;
		else *dest++ = (short) (out >> 1);
	}
}


int Write_Sound_Buffer(void *Dump_Buf)
{
/*	
	LPVOID lpvPtr1;
	DWORD dwBytes1; 
	HRESULT rval;

	if (Dump_Buf)
	{
		if (Sound_Stereo) Dump_Sound_Stereo((short *) Dump_Buf, Seg_Lenght);
		else Dump_Sound_Mono((short *) Dump_Buf, Seg_Lenght);
	}
	else
	{
		
		rval = lpDSBuffer->Lock(WP * Seg_Lenght * Bytes_Per_Unit, Seg_Lenght * Bytes_Per_Unit, &lpvPtr1, &dwBytes1, NULL, NULL, 0);

		if (rval == DSERR_BUFFERLOST)
		{
	        lpDSBuffer->Restore();
			rval = lpDSBuffer->Lock(WP * Seg_Lenght * Bytes_Per_Unit, Seg_Lenght * Bytes_Per_Unit, &lpvPtr1, &dwBytes1, NULL, NULL, 0);
		}

		if (rval == DSERR_BUFFERLOST) return 0;

		if (Sound_Stereo)
		{
			if (Have_MMX) Write_Sound_Stereo_MMX(Seg_L, Seg_R, (short *) lpvPtr1, Seg_Lenght);
			else Write_Sound_Stereo((short *) lpvPtr1, Seg_Lenght);
		}
		else
		{
			if (Have_MMX) Write_Sound_Mono_MMX(Seg_L, Seg_R, (short *) lpvPtr1, Seg_Lenght);
			else Write_Sound_Mono((short *) lpvPtr1, Seg_Lenght);
		}

		lpDSBuffer->Unlock(lpvPtr1, dwBytes1, NULL, NULL);
	}
*/
	xbox_process_audio() ;
	return 1;
}


int Clear_Sound_Buffer(void)
{
	/*
	LPVOID lpvPtr1;
	DWORD dwBytes1; 
	HRESULT rval;
	int i;

	if (!Sound_Initialised) return 0;
		
	rval = lpDSBuffer->Lock(0, Seg_Lenght * Sound_Segs * Bytes_Per_Unit, &lpvPtr1, &dwBytes1, NULL, NULL, 0);

    if (rval == DSERR_BUFFERLOST)
	{
        lpDSBuffer->Restore();
		rval = lpDSBuffer->Lock(0, Seg_Lenght * Sound_Segs * Bytes_Per_Unit, &lpvPtr1, &dwBytes1, NULL, NULL, 0);

    }

	if (rval == DS_OK)
	{
		signed short *w = (signed short *)lpvPtr1;

		for(i = 0; i < Seg_Lenght * Sound_Segs * Bytes_Per_Unit; i+= 2)
			*w++ = (signed short)0;

		rval = lpDSBuffer->Unlock(lpvPtr1, dwBytes1, NULL, NULL);

		if (rval == DS_OK) return 1;
    }
*/	
    return 0;
}


int Play_Sound(void)
{
	if (Sound_Is_Playing) 
		return 1;

	Clear_Sound_Buffer();
	return(Sound_Is_Playing = 1);
	
}


int Stop_Sound(void)
{
	Sound_Is_Playing = 0;

	return 1;
}


int Start_WAV_Dump(void)
{
	return 1;
}


int Update_WAV_Dump(void)
{
	return(1);
}


int Stop_WAV_Dump(void)
{
	return 1;
}


int Start_GYM_Dump(void)
{

	return 1;
}


int Stop_GYM_Dump(void)
{
	return 1;
}


int Start_Play_GYM(void)
{
	return 1;
}


int Stop_Play_GYM(void)
{
	return 1;
}


int GYM_Next(void)
{
	return 1;
}


int Play_GYM(void)
{
	return 1;
}

int Play_GYM_Bench(void)
{
	return 1;
}