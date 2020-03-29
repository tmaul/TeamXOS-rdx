#include <io.h>
#include "snes9x/snes9x.h"
#include "snes9x/display.h"
#include "snes9x/soundux.h"
#include "snes9x/ppu.h"
#include "snes9x/apu.h"
#include "..\..\dash.h"

static LPDIRECT3DTEXTURE8	snesTexture=NULL;
CPanel snes_RenderPanel; //used for rendering the screen

uint32 joypads [5] = {0};
int NumControllers = 2;

void S9xSetPalette ()
{
	return;
}

void S9xExtraUsage ()
{
	return;
}

uint32 S9xReadJoypad (int which1_0_to_4)
{
	if (which1_0_to_4 < NumControllers)
	{
		if (g_Gamepads[which1_0_to_4].bAnalogButtons[ XINPUT_GAMEPAD_A ])
		    joypads [which1_0_to_4] |= SNES_A_MASK;
		else
		    joypads [which1_0_to_4] &= ~SNES_A_MASK;

		if (g_Gamepads[which1_0_to_4].bAnalogButtons[ XINPUT_GAMEPAD_B ])
		    joypads [which1_0_to_4] |= SNES_B_MASK;
		else
		    joypads [which1_0_to_4] &= ~SNES_B_MASK;

		if (g_Gamepads[which1_0_to_4].bAnalogButtons[ XINPUT_GAMEPAD_Y ])
		    joypads [which1_0_to_4] |= SNES_Y_MASK;
		else
		    joypads [which1_0_to_4] &= ~SNES_Y_MASK;

		if (g_Gamepads[which1_0_to_4].bAnalogButtons[ XINPUT_GAMEPAD_X ])
		    joypads [which1_0_to_4] |= SNES_X_MASK;
		else
		    joypads [which1_0_to_4] &= ~SNES_X_MASK;

	    if (g_Gamepads[which1_0_to_4].bAnalogButtons[ XINPUT_GAMEPAD_LEFT_TRIGGER ])
			joypads [0] |= SNES_TL_MASK;
	    else
			joypads [0] &= ~SNES_TL_MASK;

	    if (g_Gamepads[which1_0_to_4].bAnalogButtons[ XINPUT_GAMEPAD_RIGHT_TRIGGER ])
			joypads [0] |= SNES_TR_MASK;
	    else
			joypads [0] &= ~SNES_TR_MASK;

	    if (g_Gamepads[which1_0_to_4].wButtons & XINPUT_GAMEPAD_BACK)
			joypads [0] |= SNES_SELECT_MASK;
	    else
			joypads [0] &= ~SNES_SELECT_MASK;

	    if (g_Gamepads[which1_0_to_4].wButtons & XINPUT_GAMEPAD_START)
			joypads [0] |= SNES_START_MASK;
	    else
			joypads [0] &= ~SNES_START_MASK;

	    if (g_Gamepads[which1_0_to_4].wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			joypads [0] |= SNES_LEFT_MASK;
	    else
			joypads [0] &= ~SNES_LEFT_MASK;

	    if (g_Gamepads[which1_0_to_4].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			joypads [0] |= SNES_RIGHT_MASK;
	    else
			joypads [0] &= ~SNES_RIGHT_MASK;

	    if (g_Gamepads[which1_0_to_4].wButtons & XINPUT_GAMEPAD_DPAD_UP)
			joypads [0] |= SNES_UP_MASK;
	    else
			joypads [0] &= ~SNES_UP_MASK;

	    if (g_Gamepads[which1_0_to_4].wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			joypads [0] |= SNES_DOWN_MASK;
	    else
			joypads [0] &= ~SNES_DOWN_MASK;

		return (0x80000000 | joypads [which1_0_to_4]);
	}
    return (0);
}

bool8 S9xReadMousePosition (int which1_0_to_1, int &x, int &y, uint32 &buttons)
{
	return false;
}

bool8 S9xReadSuperScopePosition (int &x, int &y, uint32 &buttons)
{
	return false;
}

void S9xInitDisplay (int argc, char **argv)
{
	snes_RenderPanel.Destroy();
	g_pd3dDevice->CreateTexture(IMAGE_WIDTH,IMAGE_HEIGHT, 1, 0, D3DFMT_LIN_R5G6B5 , NULL, &snesTexture);
	snes_RenderPanel.Create(g_pd3dDevice,snesTexture, true);
	return;
}

void S9xDeinitDisplay ()
{
	snes_RenderPanel.Destroy();
	return;
}

void S9xInitInputDevices ()
{
	return;
}

void S9xSetTitle (const char *title)
{
	return;
}

void S9xProcessEvents (bool8 block)
{
	return;
}

void S9xPutImage (int width, int height)
{
	return;
}

void S9xParseDisplayArg (char **argv, int &index, int argc)
{
	return;
}

void S9xToggleSoundChannel (int channel)
{
	return;
}


int S9xMinCommandLineArgs ()
{	
	return 1;
}


bool8 S9xLoadROMImage (const char *string)
{
	return false;
}

const char *S9xSelectFilename (const char *def, const char *dir,const char *ext, const char *title)
{
	return "d:\\mario.smc";
}

const char *S9xChooseFilename (bool8 read_only)
{
	return "d:\\mario.smc";
}

bool8 S9xOpenSnapshotFile (const char *base, bool8 read_only, STREAM *file)
{
	return false;
}

void S9xCloseSnapshotFile (STREAM file)
{
	return;
}

const char *S9xBasename (const char *filename)
{
	return "d:\\mario.smc";
}

int S9xFStrcmp (FILE *, const char *)
{
	return 0;
}

const char *S9xGetHomeDirectory ()
{
	return "d:\\";
}

const char *S9xGetSnapshotDirectory ()
{
	return "d:\\";
}

const char *S9xGetROMDirectory ()
{
	return "d:\\snesroms\\";
}
const char *S9xGetSRAMFilename ()
{
	return "d:\\mario.srm";
}

extern "C" const char *S9xGetFilename (const char *e)
{
    static char filename [_MAX_PATH + 1];
    char drive [_MAX_DRIVE + 1];
    char dir [_MAX_DIR + 1];
    char fname [_MAX_FNAME + 1];
    char ext [_MAX_EXT + 1];

    {
        _splitpath (Memory.ROMFilename, drive, dir, fname, ext);
        _makepath (filename, drive, dir, fname, e);
    }

    return (filename);
}

const char *S9xGetFilenameInc (const char *e)
{
    static char filename [_MAX_PATH + 1];
    char drive [_MAX_DRIVE + 1];
    char dir [_MAX_DIR + 1];
    char fname [_MAX_FNAME + 1];
    char ext [_MAX_EXT + 1];
    char *ptr;
    {
        _splitpath (Memory.ROMFilename, drive, dir, fname, ext);
        strcat (fname, TEXT("00/"));
        _makepath (filename, drive, dir, fname, e);
        ptr = strstr (filename, TEXT("00/"));
    }

    do
    {
        if (++*(ptr + 2) > '9')
        {
            *(ptr + 2) = '0';
            if (++*(ptr + 1) > '9')
            {
                *(ptr + 1) = '0';
                if (++*ptr > '9')
                    break;
            }
        }
    } while (_access (filename, 0) == 0);

    return (filename);
}

void S9xExit ()
{
	return;
}

void S9xMessage (int type, int number, const char *message)
{
	return;
}

void S9xLoadSDD1Data ()
{
	return;
}

void S9xSetPause (uint32 mask)
{
	return;
}

void S9xClearPause (uint32 mask)
{
	return;
}

void S9xSyncSpeed ()
{
	IPPU.FrameSkip = 0;
	IPPU.RenderThisFrame = TRUE;
	IPPU.SkippedFrames = 0;
}


extern "C" void S9xGenerateSound(void)
{
	return;
}

void S9xAutoSaveSRAM ()
{
	Memory.SaveSRAM (S9xGetFilename (".srm"));
}

void S9xParseArg (char **argv, int &i, int argc)
{
	return;
}


extern "C"
{
	char*osd_GetPackDir()
	{
		return NULL;	
	}
}

unsigned long _interval = 10;
long _buffernos = 4;
long _blocksize = 4400;
long _samplecount = 440;
long _maxsamplecount = 0;
long _buffersize = 0;

bool8 S9xOpenSoundDevice (int mode, bool8 pStereo, int BufferSize)
{
	 _interval = 20;
    if (Settings.SoundBufferSize < 1)
         Settings.SoundBufferSize = 1;
    if (Settings.SoundBufferSize > 64)
        Settings.SoundBufferSize = 64;

    _buffernos = 4 * Settings.SoundBufferSize;
    int s = (44100 * _interval * (Settings.Stereo ? 2 : 1) * 
             (Settings.SixteenBitSound ? 2 : 1)) / 1000;

    _blocksize = 64;
    while (_blocksize < s)
        _blocksize *= 2;

	so.mute_sound = FALSE;
    so.playback_rate = Settings.SoundPlaybackRate;
    so.stereo = Settings.Stereo;
    so.sixteen_bit = Settings.SixteenBitSound;
    so.buffer_size = _blocksize;
    so.encoded = FALSE;

	return (TRUE);
}

bool8 S9xInitUpdate (void)
{
	return (TRUE);
}

bool8 S9xDeinitUpdate (int Width, int Height, bool8 sixteen_bit)
{
	return (true);
}

bool JustifierOffscreen()
{
	return false;
}

void JustifierButtons(uint32& justifiers)
{
	return;
}


void OutOfMemory ()
{
	return;
}

void InitSNES(char * rom_filename)
{
	ZeroMemory (&Settings, sizeof (Settings));
	Settings.JoystickEnabled = FALSE;
	Settings.SoundPlaybackRate = 44100;
	Settings.SixteenBitSound= TRUE;
	Settings.Stereo = TRUE;
	Settings.SoundBufferSize = 4;
	Settings.CyclesPercentage = 100;
	Settings.DisableSoundEcho = FALSE;
	Settings.APUEnabled = Settings.NextAPUEnabled = TRUE;
	Settings.H_Max = SNES_CYCLES_PER_SCANLINE;
	Settings.SkipFrames = AUTO_FRAMERATE;
	Settings.ShutdownMaster = TRUE;
	Settings.FrameTimePAL = 20000;
	Settings.FrameTimeNTSC = 16667;
	Settings.FrameTime = Settings.FrameTimeNTSC;
	Settings.DisableSampleCaching = FALSE;
	Settings.DisableMasterVolume = FALSE;
	Settings.Mouse = FALSE;
	Settings.SuperScope = FALSE;
	Settings.MultiPlayer5 = FALSE;
	Settings.ControllerOption = SNES_JOYPAD;
	Settings.Mode7Interpolate=FALSE;
	Settings.Transparency = TRUE;
	Settings.SixteenBit = TRUE;
	Settings.SupportHiRes = FALSE;
	Settings.NetPlay = FALSE;
	Settings.ServerName [0] = 0;
	Settings.ThreadSound = FALSE;
	Settings.AutoSaveDelay = 30;
	Settings.ApplyCheats = FALSE;
	Settings.TurboMode = FALSE;
	Settings.TurboSkipFrames = 15;
	Settings.Transparency = TRUE;
	Settings.SixteenBit = TRUE;
	Settings.HBlankStart = (256 * Settings.H_Max) / SNES_HCOUNTER_MAX;

	if (!Memory.Init () || !S9xInitAPU())
		OutOfMemory ();
	uint32 saved_flags = CPU.Flags;
	S9xSetRenderPixelFormat (RGB565);


	if (rom_filename)
	{
		if (!Memory.LoadROM (rom_filename))
		{
			char dir [_MAX_DIR];
			char drive [_MAX_DRIVE];
			char name [_MAX_FNAME];
			char ext [_MAX_EXT];
			char fname [_MAX_PATH];

			_splitpath (rom_filename, drive, dir, name, ext);
			_makepath (fname, drive, dir, name, ext);

			strcpy (fname, S9xGetROMDirectory ());
			strcat (fname, SLASH_STR);
			strcat (fname, name);
			if (ext [0])
			{
				strcat (fname, ".");
				strcat (fname, ext);
			}
			_splitpath (fname, drive, dir, name, ext);
			_makepath (fname, drive, dir, name, ext);
			if (!Memory.LoadROM (fname))
			{
				printf ("Error opening: %s\n", rom_filename);
				exit (1);
			}
		}
		Memory.LoadSRAM (S9xGetSRAMFilename ());
	}
	else
	{
		S9xReset ();
		Settings.Paused |= 2;
	}
	CPU.Flags = saved_flags;

	S9xInitInputDevices ();

	S9xInitDisplay (NULL, NULL);

	GFX.Screen = new byte[(IMAGE_HEIGHT*IMAGE_WIDTH)*2];
    GFX.SubScreen=new byte[(IMAGE_HEIGHT*IMAGE_WIDTH)*2];;
    GFX.Pitch=IMAGE_WIDTH*2;
    GFX.ZBuffer=new byte[(GFX.Pitch * IMAGE_HEIGHT)];
    GFX.SubZBuffer=new byte[ (GFX.Pitch * IMAGE_HEIGHT)];
	if (!S9xGraphicsInit ())
		OutOfMemory ();

	(void) S9xInitSound (Settings.SoundPlaybackRate, Settings.Stereo,
		Settings.SoundBufferSize);
//	if (!Settings.APUEnabled)
//		S9xSetSoundMute (TRUE);

/*	if (snapshot_filename)
	{
		int Flags = CPU.Flags & (DEBUG_MODE_FLAG | TRACE_FLAG);
		if (!S9xLoadSnapshot (snapshot_filename))
			exit (1);
		CPU.Flags |= Flags;
	}
*/
	sprintf (String, "\"%s\" %s: %s", Memory.ROMName, TITLE, VERSION);
	S9xSetTitle (String);


	if (!Settings.APUEnabled)
		S9xSetSoundMute (FALSE);
	return;
}


void ExitSNES()
{
  if(Settings.SPC7110)
    (*CleanUp7110)();

  	delete [] GFX.Screen;
	delete [] GFX.SubScreen;
    delete [] GFX.ZBuffer;
    delete [] GFX.SubZBuffer;

	S9xSetSoundMute (TRUE);
    S9xDeinitDisplay ();
	S9xGraphicsDeinit();
    Memory.SaveSRAM (S9xGetFilename (".srm"));
   // S9xSaveCheatFile (S9xGetFilename (".cht"));
    Memory.Deinit();
    S9xDeinitAPU();
}

void FrameSNES()
{
	S9xMainLoop();
}

int SnesScreenSwizzle()
{
    D3DLOCKED_RECT lock={0,NULL};
	unsigned char *dest=NULL;
	int y=0,line=0;
	unsigned short *ps=NULL;

	snesTexture->LockRect(0,&lock,NULL,0); 
	if (lock.pBits==NULL) 
		return 1;

	dest=(unsigned char *)lock.pBits;
	ps=(unsigned short*)GFX.Screen;

	for (y=0; y<IMAGE_HEIGHT; y++,line+=lock.Pitch)
	{
		int x=0;
		int addr=line;

		for (x=0; x<IMAGE_WIDTH; x++,addr+=2)
		{
			*(unsigned int *)(dest+addr)=*ps++;
		}
	}
	snesTexture->UnlockRect(0);
	return 0;
}

void RenderSNES()
{
	S9xMixSamples((uint8*)DSoundNext,DSoundSeg*2);
	SnesScreenSwizzle();
	snes_RenderPanel.Render(32,24,576,432,1,0);
}