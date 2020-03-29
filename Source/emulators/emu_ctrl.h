/////
//
//	This code is part of the rdx project. (c) Team Xos
//
//  Team xos grant you the right to use this code in any non commerical way,
//  under the condidtion that you release your source code with any compiled binaries
//
//  If you make any useful changes to the code you are encouraged to email to us
//  for inclusion in a future version. Full credit will be given.
//
//  rdxdev@gmail.com
//

#pragma once 

#include <xtl.h>
#include "..\dash.h"

extern Menu *NESMenu;
extern Menu *SNESMenu;
extern Menu *MDMenu;
extern Menu *SMSMenu;

// DSound.cpp:
int DSoundInit();
void DSoundExit();
int DSoundUpdate();
extern short *DSoundNext; // Buffer for next sound data to put in loop
extern int DSoundSeg; // Seg length in samples

extern char RomName[260];

// 
void InitGens(char * romfilename);
void RenderGens();
void ExitGens();
void GensFrame();
// end gens

//
void InitSMS(char * romfilename);
void ExitSMS();
void FrameSMS();
void RenderSMS(); 
// end sms

//
void InitSNES(char * rom_filename);
void FrameSNES();
void RenderSNES();
void ExitSNES();
//end snes

//
void InitUFC(char * romfilename);
void FrameUFC();
void RenderUFC();
void ExitUFC();
//end snes

#define	GAMESELECT 1
#define	CONFIGSCREEN 2
#define	EMULATING 3
#define CREDITS 4

//ATARI
#define SYS2600 0
#define SYS5800 1
#define SYS7400 2
#define SYSLYNX 3
#define SYSJAG 4
// SEGA
#define SYSSMS 5
#define SYSSGG 6
#define SYSMD 7
#define SYSSATURN 8
//NINTENDO
#define SYSGB 9
#define SYSGBC 10
#define SYSGBA 11
#define SYSNES 12
#define SYSSNES 13
//SNK
#define SYSNGP 14
#define SYSNGCD 15
//DEC?
#define SYSPCE 16
#define SYSSPCE 17
//SONY
#define SYSPSX 18

   typedef struct {
		void (*Init)(char *);
        void (*Frame)(void);
        void (*Render)(void);
		void (*Exit)(void);
		WCHAR * title;
   } sys_driver;


void DoEmulatorMenu(int system);