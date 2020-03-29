
#include "emu_ctrl.h"

#ifndef _XBOX
#pragma warning (disable:4201)
#include <mmsystem.h>
#include <dsound.h>
#endif

static IDirectSound *DSound=NULL;
static IDirectSoundBuffer *LoopBuffer=NULL;
static int LoopLen=0,LoopWrite=0; // Next position in loop to write

short *DSoundNext=NULL; // Buffer for next sound data to put in loop
int DSoundSeg=0; // Seg length in samples

static int LoopBlank()
{
	void *mema=NULL,*memb=NULL;
	DWORD sizea=0,sizeb=0;

	LoopBuffer->Lock(0,LoopLen<<2, &mema,&sizea, &memb,&sizeb, 0);

	if (mema) memset(mema,0,sizea);

	LoopBuffer->Unlock(mema,sizea, memb,sizeb);

	return 0;
}

int DSoundInit()
{
	DSBUFFERDESC dsbd;
	WAVEFORMATEX wfx;

	memset(&dsbd,0,sizeof(dsbd));
	memset(&wfx,0,sizeof(wfx));

	// Make wave format:
	wfx.cbSize = 0;
	wfx.wFormatTag=WAVE_FORMAT_PCM;
	wfx.nChannels=2; // Stereo
	wfx.nSamplesPerSec=44100;
	wfx.wBitsPerSample=16;
	wfx.nBlockAlign=(WORD)((wfx.nChannels*wfx.wBitsPerSample)>>3);
	wfx.nAvgBytesPerSec=wfx.nBlockAlign*wfx.nSamplesPerSec;

	// Make buffer for the next seg to put into the loop:
	DSoundNext=(short *)malloc(DSoundSeg<<2); 
	if (DSoundNext==NULL) 
		return 1;
	memset(DSoundNext,0,DSoundSeg<<2);

	// Create the DirectSound interface:
	DirectSoundCreate(NULL,&DSound,NULL);
	if (DSound==NULL) 
		return 1;

	DSMIXBINVOLUMEPAIR dsmbvp[8] = {
	{DSMIXBIN_FRONT_LEFT, DSBVOLUME_MAX},   // left channel
	{DSMIXBIN_FRONT_RIGHT, DSBVOLUME_MAX},  // right channel
	{DSMIXBIN_FRONT_CENTER, DSBVOLUME_MAX}, // left channel
	{DSMIXBIN_FRONT_CENTER, DSBVOLUME_MAX}, // right channel
	{DSMIXBIN_BACK_LEFT, DSBVOLUME_MAX},    // left channel
	{DSMIXBIN_BACK_RIGHT, DSBVOLUME_MAX},   // right channel
	{DSMIXBIN_LOW_FREQUENCY, DSBVOLUME_MAX},    // left channel
	{DSMIXBIN_LOW_FREQUENCY, DSBVOLUME_MAX}};   // right channel

	LoopLen=DSoundSeg<<1; // 2 segs

	// Create the looping buffer:
	dsbd.dwSize=sizeof(dsbd);
	dsbd.dwBufferBytes=LoopLen<<2;
	dsbd.lpwfxFormat=&wfx;

	DSound->CreateSoundBuffer(&dsbd,&LoopBuffer,NULL);
	if (LoopBuffer==NULL) 
		return 1;

	LoopBlank();
	LoopBuffer->Play(0,0,DSBPLAY_LOOPING);
	return 0;
}

void DSoundExit()
{
	if (LoopBuffer) 
		LoopBuffer->Stop();
	SAFE_RELEASE(LoopBuffer)
	SAFE_RELEASE(DSound)
	free(DSoundNext); 
	DSoundNext=NULL;
}

static int WriteSeg()
{
	void *mema=NULL,*memb=NULL;
	DWORD sizea=0,sizeb=0;

	// Lock the segment at 'LoopWrite' and copy the next segment in
	LoopBuffer->Lock(LoopWrite<<2,DSoundSeg<<2, &mema,&sizea, &memb,&sizeb, 0);

	if (mema) 
		memcpy(mema,DSoundNext,sizea);

	LoopBuffer->Unlock(mema,sizea, memb,0);

	return 0;
}

int DSoundUpdate()
{
	DWORD play=0;
	int pos=0;

	if (LoopBuffer==NULL) 
		return 1;

	LoopBuffer->GetCurrentPosition(&play,NULL);
	pos=play>>2;

	// 'LoopWrite' is the next seg in the loop that we want to write
	// First check that the sound 'play' pointer has moved out of it:
	if (pos>=LoopWrite && pos<LoopWrite+DSoundSeg) 
		return 1; // No, it hasn't

	WriteSeg();

	// Advance LoopWrite to next seg:
	LoopWrite+=DSoundSeg; 
	if (LoopWrite+DSoundSeg>LoopLen) 
		LoopWrite=0;

	return 0;
}
