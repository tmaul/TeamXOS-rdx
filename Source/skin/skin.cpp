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

#define WIDTH 50
#define HEIGHT 50
#define SIZE WIDTH*HEIGHT

LPDIRECT3DTEXTURE8 bg;

static int pixel[SIZE];

CPanel vis_RenderPanel;

VOID DrawRect( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2,
			  DWORD dwFillColor, DWORD dwOutlineColor )
{
	D3DXVECTOR4 v[4];
	v[0] = D3DXVECTOR4( x1-0.5f, y1-0.5f, 0.0f, 1.0f );
	v[1] = D3DXVECTOR4( x2-0.5f, y1-0.5f, 0.0f, 1.0f );
	v[2] = D3DXVECTOR4( x2-0.5f, y2-0.5f, 0.0f, 1.0f );
	v[3] = D3DXVECTOR4( x1-0.5f, y2-0.5f, 0.0f, 1.0f );

	// Set render states
	D3DDevice::SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	D3DDevice::SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
	D3DDevice::SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	D3DDevice::SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
	D3DDevice::SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	D3DDevice::SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	D3DDevice::SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	D3DDevice::SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	D3DDevice::SetVertexShader( D3DFVF_XYZRHW );

	// Draw rectangle inside
	D3DDevice::SetRenderState( D3DRS_TEXTUREFACTOR, dwFillColor );
	D3DDevice::DrawVerticesUP( D3DPT_QUADLIST, 4, v, sizeof(D3DXVECTOR4) );

	// Draw rectangle outline
	D3DDevice::SetRenderState( D3DRS_TEXTUREFACTOR, dwOutlineColor );
	D3DDevice::DrawVerticesUP( D3DPT_LINELOOP, 4, v, sizeof(D3DXVECTOR4) );
}
// Vertex Structure
struct stVertex {
	FLOAT x, y, z, rhw;
	FLOAT tu, tv;
};


void draw_tex(LPDIRECT3DTEXTURE8 tex)
{
	//my3D.Device()->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0 );
	float fLeft   = 0;
	float fRight  = 640;
	float fTop    = 0;
	float fBottom = 480;

	stVertex myVerts[] = {
		{ fLeft,  fTop,    0, 1, 0, 0 },
		{ fRight, fTop,    0, 1, 1, 0 },
		{ fLeft,  fBottom, 0, 1, 0, 1 },

		{ fLeft,  fBottom, 0, 1, 0, 1 },
		{ fRight, fTop,    0, 1, 1, 0 },
		{ fRight, fBottom, 0, 1, 1, 1 },
	};

	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_NONE );
	g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	g_pd3dDevice->SetTexture( 0, tex );
	g_pd3dDevice->DrawVerticesUP( D3DPT_TRIANGLESTRIP, 6, &myVerts, sizeof(stVertex) );
}




void Skin_Init()
{
	D3DXCreateTextureFromFile(g_pd3dDevice, "d:\\media\\bg.png", &bg);
	ptc_open("unused string",WIDTH,HEIGHT); // open PTC with height and texture
	vis_RenderPanel.Create(g_pd3dDevice,ptc_gettexture(), false);
}

void Skin_Exit()
{

}

void Skin_BG()
{
	draw_tex(bg);
}

void Skin_Player()
{
	DrawRect(370,15,625,145,menu_bgcol,menu_framecol);
	ptc_update(pixel);
	vis_RenderPanel.Render(372,15,253,129,1,0);
}

void Skin_Info()
{
	DrawRect(15,15,355,145,menu_bgcol,menu_framecol);
	Skin_Player();
}

void Skin_MainMenu()
{
	DrawRect(15,160,625,400,menu_bgcol,menu_framecol);
}

void Skin_BottomBar()
{
	DrawRect(-1,415,640,465,menu_bgcol,menu_framecol);
}


void Skin_Emumenu()
{
	DrawRect(15,15,625,45,menu_bgcol,menu_framecol);
	DrawRect(15,60,625,400,menu_bgcol,menu_framecol);
	Skin_BottomBar();
}

void Skin_Fileman()
{
}

void Skin_Saveman()
{
}

