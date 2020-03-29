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

#include "..\dash.h"

#define MODE_SYSINFO 1
#define MODE_MUSICVIS 2

static int info_mode = MODE_SYSINFO;

int cputemp_render()
{
	ULONG uDataValue;
	HalReadSMBusValue(0x20, 0x09, FALSE,(LPBYTE) &uDataValue); 
	WCHAR strOut[1024];
	wsprintfW(strOut, L"CPU Temp: %d ", uDataValue  );
	smallfont.Begin();
	smallfont.DrawText(30, 30,0xffffffff,strOut);
	smallfont.End();

 	 // ( strOut, 0xffffffff, 30, 30 );
	return (int)uDataValue;
}

int systemp_render()
{
	ULONG uDataValue;
	HalReadSMBusValue(0x20, 0x0A, FALSE,(LPBYTE) &uDataValue);
	WCHAR strOut[1024];
	wsprintfW(strOut, L"Sys Temp: %d ", uDataValue  );
	smallfont.Begin();
	smallfont.DrawText(30, 50,0xffffffff,strOut);
	smallfont.End();
	return (int)uDataValue;
}

void memfree_render()
{
	MEMORYSTATUS stat;
	GlobalMemoryStatus( &stat );
	WCHAR strOut[1024];
	wsprintfW(strOut, L"%4d free", stat.dwAvailPhys  );
	smallfont.Begin();
	smallfont.DrawText(30, 70,0xffffffff,strOut);
	smallfont.End();
	return;
}

void info_render()
{
	Skin_Info();
	switch (info_mode)
	{
	case MODE_SYSINFO:
		cputemp_render();
		systemp_render();
		memfree_render();
		break;
	case MODE_MUSICVIS:
		break;
	}
}