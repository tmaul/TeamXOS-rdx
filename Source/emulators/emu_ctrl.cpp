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

#include "emu_ctrl.h"

Menu *NESMenu;
Menu *SNESMenu;
Menu *MDMenu;
Menu *SMSMenu;

sys_driver sys_driver_list[19]= // see app.h for system defines
{
	{NULL,NULL,NULL,NULL,L""}, //0
	{NULL,NULL,NULL,NULL,L""}, //0
	{NULL,NULL,NULL,NULL,L""}, //0
	{NULL,NULL,NULL,NULL,L""}, //0
	{NULL,NULL,NULL,NULL,L""}, //4
	{InitSMS,FrameSMS,RenderSMS,ExitSMS,L"Sega Master System"}, // 5 SMS
	{NULL,NULL,NULL,NULL,L""}, //0
	{InitGens,GensFrame,RenderGens,ExitGens,L"Sega Megadrive"}, // 7 MD
	{NULL,NULL,NULL,NULL,L""}, //0
	{NULL,NULL,NULL,NULL,L""}, //0
	{NULL,NULL,NULL,NULL,L""}, //10
	{NULL,NULL,NULL,NULL,L""}, //11
	{InitUFC,FrameUFC,RenderUFC, ExitUFC,L"Nentendo Entertainment System"},
	{InitSNES,FrameSNES, RenderSNES, ExitSNES,L"Super Nintendo Enertainment System"}, //13 snes
	{NULL,NULL,NULL,NULL,L""}, //0
	{NULL,NULL,NULL,NULL,L""}, //0
	{NULL,NULL,NULL,NULL,L""}, //0
	{NULL,NULL,NULL,NULL,L""}, //0
	{NULL,NULL,NULL,NULL,L""}, //0
};

void emu_main(int currentsystem,char * romname)
{
	int exit_request =0;
	DSoundSeg=735;
	DSoundInit();
	(*sys_driver_list[currentsystem].Init)(romname);
	g_pd3dDevice->SetFlickerFilter(0);
	while (!exit_request){
		exit_request = g_Gamepads[0].wPressedButtons&XINPUT_GAMEPAD_RIGHT_THUMB;
		Input_Frame();
		(*sys_driver_list[currentsystem].Frame)();
		while (DSoundUpdate()) 
		{ 
			Sleep(1); 
		}
		while (DSoundUpdate()==0) 
		{		
		}
		g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,0x00000000, 1.0f, 0L );
		(*sys_driver_list[currentsystem].Render)();
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

	}
	(*sys_driver_list[currentsystem].Exit)();
	memset(DSoundNext,0,DSoundSeg<<2);
	DSoundExit();
	g_pd3dDevice->SetFlickerFilter(5);
}

void emumenu_render(Menu * rendermenu, int system)
{
	WCHAR	   m_currentname[MAX_PATH];	// Game	name written to	a string
	Skin_BG();
	Skin_Emumenu();

	int drawcount=0;

	bigfont.Begin();
	bigfont.DrawText(45,18,menu_titlecol,sys_driver_list[system].title);

	for (int i =0 ; i < rendermenu->Count(); i++ )
	{
		if ((i>=rendermenu->vis_start)&&(i<=rendermenu->vis_end))
		{
			if (i==rendermenu->Selected_Item)
			{
				swprintf( m_currentname, L"");
				swprintf( m_currentname, L"%S",rendermenu->Items[i]->Title);
				bigfont.DrawText(45, 80+(23*drawcount),sel_itemcol,m_currentname);
			}
			else 
			{
				swprintf( m_currentname, L"");
				swprintf( m_currentname, L"%S", rendermenu->Items[i]->Title);
				bigfont.DrawText(45, 80+(23*drawcount),itemcol,m_currentname);
			}
			drawcount++;
		}
	}
	bigfont.End();
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

void emumenu_control(Menu * rendermenu, int system)
{
	static int i =0;

	if(g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_UP)
	{
		if (i==3)
		{
			if ((rendermenu->Selected_Item - 1)>=0)
			{
				rendermenu->Selected_Item -= 1;
			}
			i=0;
		}
		else
		{
			i++; 
		}
	}
	else if(g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
	{
		if (i==3)
		{
			if ((rendermenu->Selected_Item + 1)<(rendermenu->Count()))
			{
				rendermenu->Selected_Item += 1;
			}
			i=0;
		}
		else
		{
			i++;
		}
	}

	rendermenu->vis_start = rendermenu->Selected_Item -6;
	rendermenu->vis_end = rendermenu->Selected_Item + 6;

	if (rendermenu->Count() >=14)
	{
		if (rendermenu->vis_start <= 0)
		{
			rendermenu->vis_end-=rendermenu->vis_start;
			//rendermenu->vis_start=0;
		}

		if (rendermenu->vis_end >= rendermenu->Count())
		{
			rendermenu->vis_start += (rendermenu->vis_end -rendermenu->Count());
			if (rendermenu->vis_start <= 0)
				rendermenu->vis_start = 0;
			rendermenu->vis_end=rendermenu->Count();
		}
	}
	else
	{
		rendermenu->vis_start=0;
		rendermenu->vis_end=rendermenu->Count();
	}

			if(g_Gamepads[0].bPressedAnalogButtons[XINPUT_GAMEPAD_A])
		{
			emu_main(system,rendermenu->Items[rendermenu->Selected_Item]->Filename);
		}
}

void DoEmulatorMenu(int system)
{
	int exitmenu=0;
	Menu *localmenu;

	switch (system)
	{
	case SYSSMS:
		localmenu=SMSMenu;
		break;
	case SYSMD:
		localmenu=MDMenu;
		break;
	case SYSNES:
		localmenu=NESMenu;
		break;
	case SYSSNES:
		localmenu=SNESMenu;
		break;
	}
	while (!exitmenu)
	{
		Input_Frame();
		emumenu_control(localmenu,system);
		emumenu_render(localmenu,system);
		if(g_Gamepads[0].wPressedButtons & XINPUT_GAMEPAD_BACK)
		{
			exitmenu=1;
		}
	}

	return;
}
