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

// savegame manager should go here :-p

#include "..\dash.h"

void DoSaveman()
{
	bool exit=0;
	while(!exit)
	{
		Input_Frame();
		Skin_BG();
		Skin_Saveman();
		if(g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_BACK)
		{
			exit=1;
		}
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
}