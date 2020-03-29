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

#include <xtl.h>
#include "dash.h"

static int currentmenu=0;

LPDIRECT3DDEVICE8  g_pd3dDevice = NULL;
LPDIRECT3DSURFACE8  g_pBackBuffer =NULL;
D3DPRESENT_PARAMETERS m_d3dpp;
LPDIRECT3D8           m_pD3D= NULL;              
LPDIRECT3DDEVICE8     m_pd3dDevice= NULL;        
LPDIRECT3DSURFACE8    m_pBackBuffer= NULL;      
LPDIRECT3DSURFACE8    m_pDepthBuffer= NULL;      

static bool borednow=0; // set this to 1 to quit to dash . . .
Menu *MenusNew[8];

XKUtils m_xkutils;

CXBFont smallfont;
CXBFont bigfont;

void mainmenu_control()
{
	static int i =0;

	if (m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT)  // playlist next
	{
		Playlist_NextTrack();
	}

	if (m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)  //playlist prev
	{
		Playlist_PrevTrack();
	}

	if(m_DefaultGamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) // menu up
	{
		if (i==3)
		{
			if ((MenusNew[currentmenu]->Selected_Item - 1)>=0)
			{
				MenusNew[currentmenu]->Selected_Item -= 1;
			}
			i=0;
		}
		else
		{
			i++; 
		}
	}
	else if(g_Gamepads[0].wButtons & XINPUT_GAMEPAD_DPAD_DOWN) //menu down
	{
		if (i==3)
		{
			if ((MenusNew[currentmenu]->Selected_Item + 1)<(MenusNew[currentmenu]->Count()))
			{
				MenusNew[currentmenu]->Selected_Item += 1;
			}
			i=0;
		}
		else
		{
			i++;
		}
	}

	MenusNew[currentmenu]->vis_start = MenusNew[currentmenu]->Selected_Item -3;
	MenusNew[currentmenu]->vis_end = MenusNew[currentmenu]->Selected_Item + 3;

	if (MenusNew[currentmenu]->Count() >=8)
	{
		if (MenusNew[currentmenu]->vis_start <= 0)
		{
			MenusNew[currentmenu]->vis_end-=MenusNew[currentmenu]->vis_start;
			//MenusNew[currentmenu]->vis_start=0;
		}

		if (MenusNew[currentmenu]->vis_end >= MenusNew[currentmenu]->Count())
		{
			MenusNew[currentmenu]->vis_start += (MenusNew[currentmenu]->vis_end -MenusNew[currentmenu]->Count());
			if (MenusNew[currentmenu]->vis_start <= 0)
				MenusNew[currentmenu]->vis_start = 0;
			MenusNew[currentmenu]->vis_end=MenusNew[currentmenu]->Count();
		}
	}
	else
	{
		MenusNew[currentmenu]->vis_start=0;
		MenusNew[currentmenu]->vis_end=MenusNew[currentmenu]->Count();
	}

	if(m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_A]) //menu select/ launch
	{
		switch(MenusNew[currentmenu]->Items[MenusNew[currentmenu]->Selected_Item]->Action)
		{
		case ACTION_XBELAUNCH:
			LaunchMyXBE(MenusNew[currentmenu]->Items[MenusNew[currentmenu]->Selected_Item]->Filename,NULL);
			break;
		case ACTION_POWERDOWN:
			m_xkutils.XBOXPowerOff();
			break;
		case ACTION_REBOOT:
			m_xkutils.XBOXReset();
			break;
		case ACTION_SWITCHMENU:
			currentmenu=MenusNew[currentmenu]->Items[MenusNew[currentmenu]->Selected_Item]->TitleID;
			break;
		case ACTION_INTERNAL_EMULATOR:
			DoEmulatorMenu(MenusNew[currentmenu]->Items[MenusNew[currentmenu]->Selected_Item]->TitleID);
			break;
		case ACTION_SAVEMANAGER:
			DoSaveman();
			break;
		case ACTION_FILEMANAGER:
			DoFileman();
			break;
		}
	}

	if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)
	{
		if (currentmenu!=0)
		{
			currentmenu = MenusNew[currentmenu]->Parent;
		}
	}
}

void mainmenu_render()
{
	WCHAR	   m_currentname[256];	// Game	name written to	a string
	swprintf( m_currentname, L"");  // put menu title here

	Skin_BG();
	info_render();
	Skin_MainMenu();
	Skin_BottomBar();
	bigfont.Begin();
	bigfont.DrawText(45, 160,menu_titlecol,m_currentname);
	int drawcount=0;

	for (int i =0 ; i < MenusNew[currentmenu]->Count(); i++ )
	{
		if ((i>=MenusNew[currentmenu]->vis_start)&&(i<=MenusNew[currentmenu]->vis_end))
		{
			if (i==MenusNew[currentmenu]->Selected_Item)
			{
				swprintf( m_currentname, L"");
				swprintf( m_currentname, L"%S",MenusNew[currentmenu]->Items[i]->Title);
				bigfont.DrawText(45, 180+(23*drawcount) ,sel_itemcol,m_currentname);
			}
			else 
			{
				swprintf( m_currentname, L"");
				swprintf( m_currentname, L"%S", MenusNew[currentmenu]->Items[i]->Title);
				bigfont.DrawText(45, 180+(23*drawcount) ,itemcol,m_currentname);
			}
			drawcount++;
		}
	}
	bigfont.End();
}

void D3D_Init()
{
	ZeroMemory( &m_d3dpp, sizeof(m_d3dpp) );
	m_d3dpp.BackBufferWidth        = 640;
	m_d3dpp.BackBufferHeight       = 480;
	m_d3dpp.BackBufferFormat       = D3DFMT_A8R8G8B8;
	m_d3dpp.BackBufferCount        = 1;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;

	m_pD3D = Direct3DCreate8(D3D_SDK_VERSION);
	m_pD3D->CreateDevice( 0, D3DDEVTYPE_HAL, NULL,D3DCREATE_HARDWARE_VERTEXPROCESSING,&m_d3dpp, &m_pd3dDevice );
	g_pd3dDevice = m_pd3dDevice;

	m_pd3dDevice->GetDepthStencilSurface( &m_pDepthBuffer );
	m_pd3dDevice->GetBackBuffer( 0, 0, &m_pBackBuffer );
	m_pd3dDevice->GetBackBuffer(0,D3DBACKBUFFER_TYPE_MONO,&g_pBackBuffer);

	//check	for	pal60 mode
	if(XGetVideoStandard() == XC_VIDEO_STANDARD_PAL_I)
	{
		//get supported	video flags
		DWORD videoFlags = XGetVideoFlags();

		//set pal60	if available.
		if(videoFlags &	XC_VIDEO_FLAGS_PAL_60Hz)
			m_d3dpp.FullScreen_RefreshRateInHz = 60;
		else
		{
			m_d3dpp.FullScreen_RefreshRateInHz = 50;
		}
	}
	Font_Init();
	g_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,0x00000000, 1.0f, 0L );
	Font_Output( L"Loading, Please wait . . . ", 0xffffffff, 150, 65 );
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	m_pd3dDevice->SetFlickerFilter(5);
	m_xkutils.DVDDisableEjectReset();
	XSetFileCacheSize(0x10000);
	//XMountUtilityDrive(TRUE);

}

void D3D_Exit()
{
	SAFE_RELEASE( m_pd3dDevice );
	SAFE_RELEASE( m_pD3D );
}

void __cdecl main()
{
	D3D_Init();

	LoadConfig();

	if(	FAILED(	smallfont.Create("smallfont.xpr" ) ) )
		return;

	if(	FAILED(	bigfont.Create("bigfont.xpr" ) ) )
		return;

	Input_Init();

	Skin_Init();

	while (!borednow)
	{ 
		Input_Frame();
		mainmenu_control();
		mainmenu_render();	
		g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}

	Input_Exit();
	D3D_Exit();

	LD_LAUNCH_DASHBOARD ldld;
	ZeroMemory(&ldld, sizeof(LD_LAUNCH_DASHBOARD));
	ldld.dwReason = XLD_LAUNCH_DASHBOARD_MAIN_MENU;
	XLaunchNewImage(NULL, (PLAUNCH_DATA)&ldld);
}




