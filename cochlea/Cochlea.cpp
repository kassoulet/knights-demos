/*-----------------------------------------------------------------------------
	Cochlea
	(c) KNIGHTS 1999

	Coded by Gautier 'Impulse' PORTET [impulse@citeweb.net]
	
	december 1999 (based on RubK)

  -----------------------------------------------------------------------------*/

#define D3D_OVERLOADS
#define STRICT



#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include "D3DTextr.h"
#include "D3DUtil.h"
#include "D3DMath.h"
#include "resource.h"

#include "cochlea.xm.c"

#define MUSIC 1

#ifdef MUSIC
#include "MidasDLL.h"
MIDASmodule module;                     
MIDASmodulePlayHandle playHandle;       
#endif


typedef unsigned short word;

//-----------------------------------------------------------------------------
// Declare the application globals for use in WinMain.cpp
//-----------------------------------------------------------------------------
TCHAR* g_strAppTitle       = TEXT( "Cochlea" );
BOOL   g_bAppUseZBuffer    = FALSE;
BOOL   g_bAppUseBackBuffer = TRUE;

extern HWND hWnd;

extern float VBLframe;
extern float VBLdelta;
#define RAD(x) (((double)(x))*0.02454369261)



//-----------------------------------------------------------------------------
// Function prototypes and global (or static) variables
//-----------------------------------------------------------------------------
VOID    AppPause( BOOL );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
VOID    App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
HRESULT RenderParticles( LPDIRECT3DDEVICE3 );

#define rnd() (((FLOAT)rand()-(FLOAT)rand())/(2L*RAND_MAX))
#define RND() (((FLOAT)rand())/RAND_MAX)

//-----------------------------------------------------------------------------
// Name: Particle
// Desc: Data structure for each particle
//-----------------------------------------------------------------------------

#define NUM_FIREPARTICLES 300
#define NUM_VIOLETPARTICLES 200

D3DCOLOR FireTable[256];

struct FireParticle
{
	float x;
	float y;
	float c;
	float v;
};

FireParticle fireParticle[NUM_FIREPARTICLES];
FireParticle violetParticle[NUM_VIOLETPARTICLES];

#define NUM_PARTICLES 64

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
LPDIRECT3DMATERIAL3 g_pmtrlRenderMtrl = NULL;


D3DTLVERTEX   g_Mesh[6];                 // Vertices used to render particles

int VBLsync;

int MIDASgetPosition(void)
{
  #ifdef MUSIC
    MIDASplayStatus status;
  
  /* Get playback status: */
  MIDASgetPlayStatus(playHandle, &status);
  
	VBLsync = status.syncInfo;
  
  return status.position*256+status.row;
	#else
		return 0;
  #endif
}


	void Rotozoom(IDirect3DDevice3 * pd3dDevice, char * map, float angle, float zoom, 
													float xoffset, float yoffset, D3DRECT * rect, float alpha)
	{

    D3DTLVERTEX   grille[4];  
		#define PI 3.141592654

    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture(map) );

		if (alpha==1.0)
      pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
		else
      pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_INVSRCALPHA );

		float ru[4];
		float rv[4];
		float z,a,x,y;
/*
		z = 0.8+0.3*cos(RAD(0.23*VBLframe));
		a = 0.1;
*/
		z = zoom;
		a = angle+64+32;
		x = xoffset;
		y = yoffset;

		ru[2] = 3.5+z*cos(RAD(a-64.0))		+x;
		rv[2] = 3.5+z*sin(RAD(a-64.0))		+y;
		ru[0] = 3.5+z*cos(RAD(a+0.0))	+x;
		rv[0] = 3.5+z*sin(RAD(a+0.0))	+y;
		ru[1] = 3.5+z*cos(RAD(a+64.0))	+x;
		rv[1] = 3.5+z*sin(RAD(a+64.0))	+y;
		ru[3] = 3.5+z*cos(RAD(a+128.0))	+x;
		rv[3] = 3.5+z*sin(RAD(a+128.0))	+y;

		D3DCOLOR c1,c2,c3,c4;

		a = 0.5+0.5*sin(RAD(VBLframe));
		a = alpha;

		c1 = D3DRGBA(	0.5+0.5*cos(RAD(VBLframe*0.27)),
									0.5+0.5*cos(RAD(VBLframe*0.96)),
									0.5+0.5*cos(RAD(VBLframe*7.26)),a);
				
		c2 = D3DRGBA(	0.5+0.5*cos(RAD(VBLframe*0.02)),
									0.5+0.5*cos(RAD(VBLframe*0.94)),
									0.5+0.5*cos(RAD(VBLframe*0.34)),a);

		c3 = D3DRGBA(	0.5+0.5*cos(RAD(VBLframe*5.62)),
									0.5+0.5*cos(RAD(VBLframe*4.26)),
									0.5+0.5*cos(RAD(VBLframe*0.43)),a);

		c4 = D3DRGBA(	0.5+0.5*cos(RAD(VBLframe*0.71)),
									0.5+0.5*cos(RAD(VBLframe*2.65)),
									0.5+0.5*cos(RAD(VBLframe*0.81)),a);

		c1=c2=c3=c4=-1;

		grille[0] = D3DTLVERTEX(	D3DVECTOR(rect->x1,rect->y2,0.99f), 1, 
															c1,
															D3DRGBA(1,0,0,1.0), ru[0], rv[0] );
		grille[1] = D3DTLVERTEX(	D3DVECTOR(rect->x1,rect->y1,0.99f), 1, 
															c2,
															D3DRGBA(1,0,0,1.0), ru[1], rv[1] );
		grille[2] = D3DTLVERTEX(	D3DVECTOR(rect->x2,rect->y2,0.99f), 1, 
															c3,
															D3DRGBA(1,0,0,1.0), ru[2], rv[2] );
		grille[3] = D3DTLVERTEX(	D3DVECTOR(rect->x2,rect->y1,0.99f), 1, 
															c4,
															D3DRGBA(1,0,0,1.0), ru[3], rv[3] );

    while( DDERR_WASSTILLDRAWING == pd3dDevice->DrawPrimitive(
																			D3DPT_TRIANGLESTRIP,
																			D3DFVF_TLVERTEX,
                                      grille, 
																			4,
                                      0) )
		{
			// Loop while waiting for the DrawPrim() call to succeed. We could
			// use this time to do some other processing.
		}
	}

	void DrawZoomSprite(IDirect3DDevice3 * pd3dDevice, char * sprite, float x1, float y1, float x2, float y2,float a=0, float z=1)
	{
    D3DTLVERTEX   grille[4];  
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_INVSRCALPHA );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture(sprite) );
		
		float u,v;

		u = 0.625/z/2.0;
		v = 0.9375/z/2.0;

		grille[0] = D3DTLVERTEX(	D3DVECTOR(x1,y2,1.0), 1, 
															D3DRGBA(1,1,1,a),
															D3DRGBA(1,0,0,1.0), 0.3125 - u, 0.46875 + v);
		grille[1] = D3DTLVERTEX(	D3DVECTOR(x1,y1,1.0), 1, 
															D3DRGBA(1,1,1,a),
															D3DRGBA(1,0,0,1.0), 0.3125 - u, 0.46875 - v);
		grille[2] = D3DTLVERTEX(	D3DVECTOR(x2,y2,1.0), 1, 
															D3DRGBA(1,1,1,a),
															D3DRGBA(1,0,0,1.0), 0.3125 + u, 0.46875 + v);
		grille[3] = D3DTLVERTEX(	D3DVECTOR(x2,y1,1.0), 1, 
															D3DRGBA(1,1,1,a),
															D3DRGBA(1,0,0,1.0), 0.3125 + u, 0.46875 - v);

    while( DDERR_WASSTILLDRAWING == pd3dDevice->DrawPrimitive(
																		D3DPT_TRIANGLESTRIP,
																		D3DFVF_TLVERTEX,
                                    grille, 
																		4,
                                    0) )
		{
			// Loop while waiting for the DrawPrim() call to succeed. We could
			// use this time to do some other processing.
		}
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
	}

	void DrawAddSprite(IDirect3DDevice3 * pd3dDevice, char * sprite, float x1, float y1, float x2, float y2,float a1=0, float a2=1)
	{
    D3DTLVERTEX   grille[4];  
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_ONE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_ONE );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture(sprite) );
		
		grille[0] = D3DTLVERTEX(	D3DVECTOR(x1,y2,1.0), 1, 
															D3DRGBA(a1,a1,a1,1),
															D3DRGBA(1,0,0,1.0), 0, 1 );
		grille[1] = D3DTLVERTEX(	D3DVECTOR(x1,y1,1.0), 1, 
															D3DRGBA(a1,a1,a1,1),
															D3DRGBA(1,0,0,1.0), 0, 0 );
		grille[2] = D3DTLVERTEX(	D3DVECTOR(x2,y2,1.0), 1, 
															D3DRGBA(a2,a2,a2,1),
															D3DRGBA(1,0,0,1.0), 1, 1 );
		grille[3] = D3DTLVERTEX(	D3DVECTOR(x2,y1,1.0), 1, 
															D3DRGBA(a2,a2,a2,1),
															D3DRGBA(1,0,0,1.0), 1, 0 );

    while( DDERR_WASSTILLDRAWING == pd3dDevice->DrawPrimitive(
																		D3DPT_TRIANGLESTRIP,
																		D3DFVF_TLVERTEX,
                                    grille, 
																		4,
                                    0) )
		{
			// Loop while waiting for the DrawPrim() call to succeed. We could
			// use this time to do some other processing.
		}
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
	}

	void DrawBackground(IDirect3DDevice3 * pd3dDevice, char * sprite, float zoom)
	{
		float x1,y1,x2,y2;

		x1=320-zoom*320;
		y1=240-zoom*240;
		x2=320+zoom*320;
		y2=240+zoom*240;


    D3DTLVERTEX   grille[4];  

    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture(sprite) );


    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE,   FALSE );
		
		grille[0] = D3DTLVERTEX(	D3DVECTOR(x1,y2,1.0), 1, 
															D3DRGBA(1,1,1,1),
															D3DRGBA(1,0,0,1.0), 0, 0.9375 );
		grille[1] = D3DTLVERTEX(	D3DVECTOR(x1,y1,1.0), 1, 
															D3DRGBA(1,1,1,1),
															D3DRGBA(1,0,0,1.0), 0, 0 );
		grille[2] = D3DTLVERTEX(	D3DVECTOR(x2,y2,1.0), 1, 
															D3DRGBA(1,1,1,1),
															D3DRGBA(1,0,0,1.0), 0.625, 0.9375 );
		grille[3] = D3DTLVERTEX(	D3DVECTOR(x2,y1,1.0), 1, 
															D3DRGBA(1,1,1,1),
															D3DRGBA(1,0,0,1.0), 0.625, 0 );

    while( DDERR_WASSTILLDRAWING == pd3dDevice->DrawPrimitive(
																		D3DPT_TRIANGLESTRIP,
																		D3DFVF_TLVERTEX,
                                    grille, 
																		4,
                                    0) )
		{
			// Loop while waiting for the DrawPrim() call to succeed. We could
			// use this time to do some other processing.
		}
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
	}


	void DrawRect(IDirect3DDevice3 * pd3dDevice, float x1, float y1, float x2, float y2,D3DCOLOR c)
	{
    D3DTLVERTEX   grille[4];  
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_INVSRCALPHA );
    pd3dDevice->SetTexture( 0, 0 );
		
		grille[0] = D3DTLVERTEX(	D3DVECTOR(x1,y2,1.0), 1, 
															c,
															0, 0, 1 );
		grille[1] = D3DTLVERTEX(	D3DVECTOR(x1,y1,1.0), 1, 
															c,
															0, 0, 0 );
		grille[2] = D3DTLVERTEX(	D3DVECTOR(x2,y2,1.0), 1, 
															c,
															0, 1, 1 );
		grille[3] = D3DTLVERTEX(	D3DVECTOR(x2,y1,1.0), 1, 
															c,
															0, 1, 0 );

    while( DDERR_WASSTILLDRAWING == pd3dDevice->DrawPrimitive(
																		D3DPT_TRIANGLESTRIP,
																		D3DFVF_TLVERTEX,
                                    grille, 
																		4,
                                    0) )
		{
			// Loop while waiting for the DrawPrim() call to succeed. We could
			// use this time to do some other processing.
		}
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
	}

	void DrawAddRect(IDirect3DDevice3 * pd3dDevice, float x1, float y1, float x2, float y2,D3DCOLOR c)
	{
    D3DTLVERTEX   grille[4];  
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_ONE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_ONE );
    pd3dDevice->SetTexture( 0, 0 );
		
		grille[0] = D3DTLVERTEX(	D3DVECTOR(x1,y2,1.0), 1, 
															c,
															0, 0, 1 );
		grille[1] = D3DTLVERTEX(	D3DVECTOR(x1,y1,1.0), 1, 
															c,
															0, 0, 0 );
		grille[2] = D3DTLVERTEX(	D3DVECTOR(x2,y2,1.0), 1, 
															c,
															0, 1, 1 );
		grille[3] = D3DTLVERTEX(	D3DVECTOR(x2,y1,1.0), 1, 
															c,
															0, 1, 0 );

    while( DDERR_WASSTILLDRAWING == pd3dDevice->DrawPrimitive(
																		D3DPT_TRIANGLESTRIP,
																		D3DFVF_TLVERTEX,
                                    grille, 
																		4,
                                    0) )
		{
			// Loop while waiting for the DrawPrim() call to succeed. We could
			// use this time to do some other processing.
		}
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
	}

	void DrawSubRect(IDirect3DDevice3 * pd3dDevice, float x1, float y1, float x2, float y2,D3DCOLOR c)
	{
    D3DTLVERTEX   grille[4];  
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_ZERO );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_SRCCOLOR );
    pd3dDevice->SetTexture( 0, 0 );
		
		grille[0] = D3DTLVERTEX(	D3DVECTOR(x1,y2,1.0), 1, 
															c,
															0, 0, 1 );
		grille[1] = D3DTLVERTEX(	D3DVECTOR(x1,y1,1.0), 1, 
															c,
															0, 0, 0 );
		grille[2] = D3DTLVERTEX(	D3DVECTOR(x2,y2,1.0), 1, 
															c,
															0, 1, 1 );
		grille[3] = D3DTLVERTEX(	D3DVECTOR(x2,y1,1.0), 1, 
															c,
															0, 1, 0 );

    while( DDERR_WASSTILLDRAWING == pd3dDevice->DrawPrimitive(
																		D3DPT_TRIANGLESTRIP,
																		D3DFVF_TLVERTEX,
                                    grille, 
																		4,
                                    0) )
		{
			// Loop while waiting for the DrawPrim() call to succeed. We could
			// use this time to do some other processing.
		}
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
	}


void Error(char *string, ... )
{
   char buf[256];

  va_list ap;
  va_start(ap, string);
  vsprintf(buf, string, ap);
  va_end(ap);

#ifdef MUSIC
	MIDASclose();
#endif
  MessageBox(NULL,buf,"[ERROR]",MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);
	exit(1);
}

void MIDASerror(void)
{
#ifdef MUSIC
    char        *errorMessage;


    errorMessage = MIDASgetErrorMessage(MIDASgetLastError());
    MessageBox(NULL, errorMessage, "MIDAS Error!",
        MB_APPLMODAL | MB_ICONSTOP | MB_OK);
     MIDASclose();
#endif

		exit(0);
}


//-----------------------------------------------------------------------------
// Name: App_OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT App_OneTimeSceneInit( HWND hWnd )
{
	int i;
	srand(0x9865485);
    // Load textures
    D3DTextr_CreateTexture( "light.bmp" );
    D3DTextr_CreateTexture( "mygirl.bmp" );
    D3DTextr_CreateTexture( "phantasm.bmp" );
    D3DTextr_CreateTexture( "knights.bmp" );
    D3DTextr_CreateTexture( "title.bmp" );
    D3DTextr_CreateTexture( "anim.bmp" );
    D3DTextr_CreateTexture( "flare.bmp",0,D3DTEXTR_TRANSPARENTBLACK);
    D3DTextr_CreateTexture( "violet.bmp" );
    D3DTextr_CreateTexture( "scroll.bmp" );
    D3DTextr_CreateTexture( "line.bmp" );
    D3DTextr_CreateTexture( "pub.bmp" );
    D3DTextr_CreateTexture( "back.bmp" );
//    D3DTextr_CreateTexture( ".bmp" );

		#ifdef MUSIC
    MIDASstartup();
    // MIDASconfig();

		MIDASsetOption(MIDAS_OPTION_MIXING_MODE,MIDAS_MIX_HIGH_QUALITY);
		MIDASsetOption(MIDAS_OPTION_DSOUND_MODE, MIDAS_DSOUND_PRIMARY);
		MIDASsetOption(MIDAS_OPTION_DSOUND_HWND, (DWORD) hWnd);

    if ( !MIDASinit() )
			MIDASerror();


		char str[1024];

		GetTempPath(1000,str);
		strcat(str,"cochlea.xm");
		FILE * f = fopen(str,"wb");
		fwrite(zik,1,sizeof(zik),f);
		fclose(f);

    if ( (module = MIDASloadModule(str)) == NULL )
	    MIDASerror();
	

/*
    if ( (module = MIDASloadModule("cochlea.xm")) == NULL )
	    MIDASerror();
*/		

		MIDASstartBackgroundPlay(0);
    /* Start playing the module: */
    if ( (playHandle = MIDASplayModule(module, TRUE)) == 0 )
		  MIDASerror();
		#endif



    for( i=0; i<NUM_FIREPARTICLES; i++ )
    {
			fireParticle[i].y=rand()%480;
			fireParticle[i].x=rand()%640;
			fireParticle[i].c=200+(rand()%50);
			fireParticle[i].v=(10+rand()%50)/30.0;
		}

    for( i=0; i<NUM_VIOLETPARTICLES; i++ )
    {
			violetParticle[i].y=rand()%480;
			violetParticle[i].x=rand()%200;
			violetParticle[i].c=8+(rand()%32);
			violetParticle[i].v=(10+(rand()%100))/50.0;
		}

    for( i=0; i<128; i++ )
			FireTable[i] = D3DRGBA(i/128.0,0,0,0.8);
    for( i=0; i<64; i++ )
			FireTable[128+i] = D3DRGBA(1.0,i/64.0,0,0.4);
    for( i=0; i<64; i++ )
			FireTable[128+i+64] = D3DRGBA(1.0,1.0,i/64.0,0.2);


    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: App_FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT App_FrameMove( LPDIRECT3DDEVICE3 pd3dDevice, FLOAT fTimeKey )
{
	static float old;

		VBLframe = 30.0*fTimeKey;

	VBLdelta = VBLframe-old;
	old=VBLframe;


	if (VBLdelta>100)
		VBLdelta=0;

	return S_OK;
}

void PartBegin(LPDIRECT3DDEVICE3 pd3dDevice)
{
		static float a=1.0;
		static float zt=1.0;
		static float z=0.0;
		if (MIDASgetPosition()>0x0420)
		{
			z = (MIDASgetPosition()&0xfc)-0x20;

			if (MIDASgetPosition()>=0x043b) 
				z+=3;
			if (MIDASgetPosition()>=0x043c)
				z+=3;
			if (MIDASgetPosition()>=0x043e)
				z+=3;
		}


	if (MIDASgetPosition()>0x0200)
	{
		a-=VBLdelta*0.01;
		if (a<0)
			a=0;
		DrawBackground(pd3dDevice, "mygirl.bmp", 1.0-a);
	}

	zt+=VBLdelta*0.001;

	if (MIDASgetPosition()<0x0300)
	{
		DrawAddSprite(pd3dDevice, "title.bmp",320-256*zt,240-128*zt,320+256*zt,240+128*zt,a,a);
/*
		D3DRECT r={0,0,640,480};
		Rotozoom(pd3dDevice, "back.bmp", 0, 1.2 ,0, VBLframe*0.02, &r, 1.0);

		int i;

		for (i=0;i<10;i++)
		{
			if ((rand()%100)>60)
			{
				int y = ((i*100+(int)(VBLframe*5.0)+(rand()%10))&1023)-512;
				float a = (rand()%300)/1000.0;
				DrawAddRect(pd3dDevice, 0, y, 639, y+4,D3DRGB(a,a,a));
			}
		}

//		DrawAddRect(pd3dDevice, 0, 0, 100, 100,D3DRGB(1.0,1.0,1.0));

*/
	}
	else
		DrawBackground(pd3dDevice, "mygirl.bmp", 1.0+z/32.0);


	int y;
	if (MIDASgetPosition()>0x0200)
	for(y=0;y<8;y++)
	{			
		static float x;
		if (VBLsync==1)
		{
//			x+=VBLdelta/2.0;
//			if(x>40.0)
				x=40.0;
		}		
		x-=VBLdelta/4.0;
		if(x<0.0)
			x=0.0;

		int i;
		i = 128*y+VBLframe;
		i = i&1023;
		i = 512-i;
		DrawAddSprite(pd3dDevice, "title.bmp",384-32-x,i,640-32+x,i+128,1.0-a,0.0);
		i = -i;
		DrawAddSprite(pd3dDevice, "title.bmp",384-32+x,i,640-32-x,i+128,0.0,1.0-a);
	}

}

void PartMyGirl(LPDIRECT3DDEVICE3 pd3dDevice)
{
	float x,y;
	static float dist,angle;
	static float speed;
	static float pos;

	int n = (MIDASgetPosition()&255);
	switch (n)
	{
		case 0:
		case 3:
		case 6:
		case 8:
		case 11:
		case 14:
		case 16:
		case 17:
		case 19:
		case 22:
		case 24:
		case 27:
		case 30:
		case 32:
		case 35:
		case 38:
		case 40:
		case 43:
		case 46:
		case 48:
		case 49:
		case 51:
		case 54:
		case 56:
		case 57:
		case 59:
		case 62:
			speed+= VBLdelta;
			break;
		default:
			speed-= VBLdelta*2.0;
	}

	if (speed<1.0)
		speed = 1.0;

//	pos+=VBLdelta*speed;
	pos+=VBLdelta;

	x = -320.0-320.0*sin(0.012*pos);
	y = -240.0-240.0*cos(0.021*pos);

	DrawAddSprite(pd3dDevice, "mygirl.bmp",x,y,2*1024+x,2*512+y,1.0,1.0);


	dist = 1+VBLsync;

	if ((MIDASgetPosition()>>8)>=9)
		angle+=VBLdelta;
	else
	{
		if (VBLsync==4)
			angle = 80.0;
		else
			angle = 50.0;
	}

	// Turn on alpha blending for the particles
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("light.bmp") );

    // Do the particles
    for( float j=0; j<4; j++ )
    for( float i=0; i<128; i++ )
    {
			float x,y,z,r,a;
			r = angle+1*i;
			a = 64*j+1*VBLframe+i*(dist*sin(RAD(1.0*VBLframe)));  //8.0;
			x = 320.0+ r*sin(RAD(a));
			y = 240.0+ r*cos(RAD(a));
			z = 25.0+10.0*sin(RAD(2.0*i+VBLframe));
			D3DCOLOR c;

			c = D3DRGBA(
								0.5+0.5*sin(RAD(0.21*VBLframe+4.98*i)),
								0.5+0.5*sin(RAD(3.12*VBLframe+1.28*i)),
								0.5+0.5*sin(RAD(7.94*VBLframe+2.17*i)),
								0.5+0.3*sin(RAD(1.19*VBLframe+3.14*i))
								);

		  g_Mesh[0] = D3DTLVERTEX( D3DVECTOR( x-z, y+z, 0.5 ), 0.5, c, 0, 0, 1 );
			g_Mesh[1] = D3DTLVERTEX( D3DVECTOR( x-z, y-z, 0.5 ), 0.5, c, 0, 0, 0 );
			g_Mesh[2] = D3DTLVERTEX( D3DVECTOR( x+z, y+z, 0.5 ), 0.5, c, 0, 1, 1 );
			g_Mesh[4] = D3DTLVERTEX( D3DVECTOR( x-z, y-z, 0.5 ), 0.5, c, 0, 0, 0 );
			g_Mesh[3] = D3DTLVERTEX( D3DVECTOR( x+z, y+z, 0.5 ), 0.5, c, 0, 1, 1 );
			g_Mesh[5] = D3DTLVERTEX( D3DVECTOR( x+z, y-z, 0.5 ), 0.5, c, 0, 1, 0 );

      pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX,g_Mesh, 6, 0 );

    }
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );


}

void PartScroll(LPDIRECT3DDEVICE3 pd3dDevice)
{
	int n,i;
	static float y=500;
	static float z=1.0;
	static float a=0.0;

	if (z>10)
		a+=VBLdelta*0.02;

	if (a>1.0)
		a = 1.0;

	DrawBackground(pd3dDevice, "mygirl.bmp", z);

	DrawAddSprite(pd3dDevice, "title.bmp",160-128,240-64,160+128,240+64,a,a);
	DrawAddSprite(pd3dDevice, "pub.bmp",160-128,300-16,160+128,300+16,a,a);

	if (y<-1200)
		DestroyWindow( hWnd );

	if (VBLdelta<10)
	{
		y -= 1.0*VBLdelta;
		if (z<20.0)
			z *= 1.0+0.003*VBLdelta;
	}

	n = 20.0*sin(VBLframe*0.1);

/*
		if (n<0)
		n = 0;
		for(i=1;i<4;i++)
		{
			DrawAddSprite(pd3dDevice, "scroll.bmp",320+n*i*0.1,y,320+256+n*i*0.1,y+1024,0.8/8.0,0.8/8.0);
			DrawAddSprite(pd3dDevice, "scroll.bmp",320-n*i*0.1,y,320+256-n*i*0.1,y+1024,0.8/8.0,0.8/8.0);
		}
*/
	DrawAddSprite(pd3dDevice, "scroll.bmp",320,y,320+256,y+1024,0.8,0.8);

	for(i=2;i<5;i+=1)
		DrawAddSprite(pd3dDevice, "scroll.bmp",320+160-128*i,y*i,320+160+128*i,y+1024*i,0.5/i,0.5/i);
}

struct Flare
{
	float x,y,p;
};

#define NUM_FLARES 13

Flare flares[NUM_FLARES]={{507,133},{525,117},{595,75},{326,116},{297,141},{218,169},{315,5},{311,56},{308,22},{489,148},{570,80},{290,127},{308,136}};


void PartFire(LPDIRECT3DDEVICE3 pd3dDevice)
{
	static Flare flare={0,0,1024};

	DrawBackground(pd3dDevice, "knights.bmp", 1.0);

	int n;

	for(n=0;n<NUM_FLARES;n++)
		{
/*
			flare.p+=VBLdelta*5.0;

			if (flare.p>=128.0)
			{
				flare.p=0;
				int i = rand()%NUM_FLARES; 
				flare.x=flares[i].x;
				flare.y=flares[i].y;
			}	

			int x,y,z;
			x = flare.x;
			y = flare.y;
			z = 16.0+16.0*sin(flare.p*3.1415927*2.0/256.0);
*/

			int x,y,z;
			x = flares[n].x;
			y = flares[n].y;
			z = 16.0*sin((125.2567*n+4.0*VBLframe)*3.1415927*2.0/256.0)+16.0*sin((13.259*n+1.0*VBLframe)*3.1415927*2.0/256.0);
			if (z)
				DrawAddSprite(pd3dDevice, "flare.bmp",x-z,y-z,x+z,y+z,z/32.0,z/32.0);
		}	

    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_ONE );
	  pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_ONE );

//    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("light.bmp") );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("anim.bmp") );

    // Do the particles
    for( int i=0; i<NUM_FIREPARTICLES; i++ )
//    for( int i=0; i<4; i++ )
    {
			D3DCOLOR c;
			float x,y,z;

			fireParticle[i].y-=fireParticle[i].v*VBLdelta*2.0;
			fireParticle[i].c-=fireParticle[i].v*VBLdelta*0.8*2.0;

			if (fireParticle[i].y <= 200.0)
			{
				fireParticle[i].c=100+(rand()%150);
				fireParticle[i].y=480+32/*+(rand()%50)*/;
				fireParticle[i].x=rand()%640;
				fireParticle[i].v=(10+(rand()%100))/50.0;
			}

			x = fireParticle[i].x;
			y = fireParticle[i].y;
			z = fireParticle[i].c/2.0;
//			z = 32;

			if (fireParticle[i].c > 10.0)
			{
				c = FireTable[((int)fireParticle[i].c)&255];

				float u1,u2,v1,v2;
				int f = VBLframe+i+fireParticle[i].c;
				int mx,my;
			
				mx = 32*(f&7);
				my = 32*((f>>3)&3);

				u1 = (mx+1)/256.0;
				v1 = (my+1)/128.0;

				u2 = (mx+30)/256.0;
				v2 = (my+30)/128.0;

			  g_Mesh[0] = D3DTLVERTEX( D3DVECTOR( x-z, y+z, 0.5 ), 0.5, c, 0, u1, v2 );
				g_Mesh[1] = D3DTLVERTEX( D3DVECTOR( x-z, y-z, 0.5 ), 0.5, c, 0, u1, v1 );
				g_Mesh[2] = D3DTLVERTEX( D3DVECTOR( x+z, y+z, 0.5 ), 0.5, c, 0, u2, v2 );
				g_Mesh[4] = D3DTLVERTEX( D3DVECTOR( x-z, y-z, 0.5 ), 0.5, c, 0, u1, v1 );
				g_Mesh[3] = D3DTLVERTEX( D3DVECTOR( x+z, y+z, 0.5 ), 0.5, c, 0, u2, v2 );
				g_Mesh[5] = D3DTLVERTEX( D3DVECTOR( x+z, y-z, 0.5 ), 0.5, c, 0, u2, v1 );

	      pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, g_Mesh, 6, 0 );
			}

    }



	if (MIDASgetPosition()>0x1300)
	{
		int x=0,y=0;
		static float z,a;

		static float pos=0;
		static int fade=0;

		pos+=VBLdelta*0.01;

		if (pos>=1.0)
		{
			pos=0.0;
			fade++;
			if (fade==3)
				fade = 0;
		}

		a = pos;
		z = 1.0+ 1.0*pos;

		if (fade == 0)
		{
			DrawZoomSprite(pd3dDevice, "phantasm.bmp",x,y,x+160,y+120,1.0-a,2.0*z);
			DrawZoomSprite(pd3dDevice, "mygirl.bmp",x,y,x+160,y+120,a,z);
		}
		if (fade == 1)
		{
			DrawZoomSprite(pd3dDevice, "mygirl.bmp",x,y,x+160,y+120,1.0-a,2.0*z);
			DrawZoomSprite(pd3dDevice, "knights.bmp",x,y,x+160,y+120,a,z);
		}
		if (fade == 2)
		{
			DrawZoomSprite(pd3dDevice, "knights.bmp",x,y,x+160,y+120,1.0-a,2.0*z);
			DrawZoomSprite(pd3dDevice, "phantasm.bmp",x,y,x+160,y+120,a,z);
		}
	}
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
}



void PartPhantasm(LPDIRECT3DDEVICE3 pd3dDevice)
{
//	static Flare flare={0,0,1024};

	DrawBackground(pd3dDevice, "phantasm.bmp", 1.0);

	static float flare=0.0;

	if( (MIDASgetPosition()&31)==0)
		flare=32.0;

	flare-=VBLdelta*0.5;
	
	if (flare<0)
		flare=0;
	else
	 DrawAddSprite(pd3dDevice, "flare.bmp",476-flare,207-flare,476+flare,207+flare,flare*32.0,flare*32.0);

    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_ONE );
	  pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_ONE );

	int x;
	int y;
	static float power=0.0;
	static float alpha=0.0;

	if( (MIDASgetPosition()>=0x1d00) )
	{
		power+=VBLdelta*0.5;
		alpha+=VBLdelta*0.002;
		if (alpha>0.7)
			alpha=0.7;
	}

	if ((GetAsyncKeyState(VK_UP)&0xf000))
		power+=0.1;
	if ((GetAsyncKeyState(VK_DOWN)&0xf000))
		power-=0.1;

	if ((GetAsyncKeyState(VK_RIGHT)&0xf000))
		alpha+=0.01;
	if ((GetAsyncKeyState(VK_LEFT)&0xf000))
		alpha-=0.01;

	if (power<0)
		power = 0;

	if (alpha<0)
		alpha = 0;
	if (alpha>1.0)
		alpha = 1.0;

	D3DTLVERTEX   line[2*40*40];
	D3DCOLOR c = D3DRGBA(alpha,alpha,alpha,1);

	for(y=-20;y<20;y++)
		for(x=-20;x<20;x++)
		{
			float i,j;

			i = x*40+400.0*sin((x+2*y+VBLframe)*2.0*3.1415927/256.0)*sin((7*y+5*VBLframe)*2.0*3.1415927/256.0);
			j = y*40+300.0*cos((3*x+y+VBLframe)*2.0*3.1415927/256.0)*sin((5*x+3*VBLframe)*2.0*3.1415927/256.0);
//			j = y*40+100.0*cos((2*x+y+VBLframe)*2.0*3.1415927/256.0);

			float u1,u2,v1,v2;

			u1=0;
			v1=(-VBLframe)*0.01/64.0;
			u2=1.0;
			v2=v1+power/64.0;

			line[(y+20)*80+(x+20)*2+0] = D3DTLVERTEX( D3DVECTOR( 476, 207, 0.5 ), 0.5, c, 0, u1, v1 );
			line[(y+20)*80+(x+20)*2+1] = D3DTLVERTEX( D3DVECTOR( 320+i, 240+j, 0.5 ), 0.5, c, 0, u2, v2 );
		}

		pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("line.bmp") );
		if (alpha>0)
			pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_TLVERTEX, line, 2*40*40, NULL );


    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_ONE );
	  pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_ONE );

    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("violet.bmp") );


			static float violet=0.0;
			{
				violet += VBLdelta*0.01;
				if (violet > 1.0)
					violet = 1.0;
			}

			if( (MIDASgetPosition()>=0x1d00) )
			{
				violet -= VBLdelta*0.02;
				if (violet<0.0)
					violet = 0.0;
			}

    // Do the particles
    for( int i=0; i<NUM_VIOLETPARTICLES; i++ )
//    for( int i=0; i<4; i++ )
    {
			D3DCOLOR c;
			float x,y,z;

			violetParticle[i].y-=violetParticle[i].v*VBLdelta;

			if (violetParticle[i].y <= -40.0)
			{
				if (i&1)
				  violetParticle[i].c=8+(rand()%32);
				else
				  violetParticle[i].c=8+(rand()%8);
				violetParticle[i].y=480+32+(rand()%50);
				violetParticle[i].x=40+(rand()%160);
				violetParticle[i].v=(30+(rand()%80))/20.0;   //violetParticle[i].c;
			}

			static float a=0.0;

			x = 120.0+80.0*sin((violetParticle[i].x+violetParticle[i].y+3.0*sin(VBLframe*0.01))*3.1415927*2.0/300.0);
			y = violetParticle[i].y;
			z = violetParticle[i].c;

			if (violet>0)
			{
				c = D3DRGBA(violet,violet,violet,1);

			  g_Mesh[0] = D3DTLVERTEX( D3DVECTOR( x-z, y+z, 0.5 ), 0.5, c, 0, 0, 1 );
				g_Mesh[1] = D3DTLVERTEX( D3DVECTOR( x-z, y-z, 0.5 ), 0.5, c, 0, 0, 0 );
				g_Mesh[2] = D3DTLVERTEX( D3DVECTOR( x+z, y+z, 0.5 ), 0.5, c, 0, 1, 1 );
				g_Mesh[4] = D3DTLVERTEX( D3DVECTOR( x-z, y-z, 0.5 ), 0.5, c, 0, 0, 0 );
				g_Mesh[3] = D3DTLVERTEX( D3DVECTOR( x+z, y+z, 0.5 ), 0.5, c, 0, 1, 1 );
				g_Mesh[5] = D3DTLVERTEX( D3DVECTOR( x+z, y-z, 0.5 ), 0.5, c, 0, 1, 0 );

	      pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX,g_Mesh, 6, 0 );
			}

    }

		if (violet>0)
			for (int n=0;n<2048;n+=100)
			{
				int y = 1024-((n+(int)VBLframe*5)&2047);
				DrawAddSprite(pd3dDevice, "light.bmp",120-16,y-16,120+16,y+16,1.0,1.0);
			}

    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );


/*
{
		LPDIRECTDRAWSURFACE4 pddsRenderSurface;
		if( FAILED( pd3dDevice->GetRenderTarget( &pddsRenderSurface ) ) )
				return;

		char str[20];

		sprintf(str,"%i",(int) (400+power*1600));

		// Get a DC for the surface. Then, write out the buffer
		HDC hDC;
		if( SUCCEEDED( pddsRenderSurface->GetDC(&hDC) ) )
		{
				SetTextColor( hDC, RGB(255,255,0) );
				SetBkMode( hDC, TRANSPARENT );
				ExtTextOut( hDC, 0, 20, 0, NULL, str, strlen(str), NULL );
		
				pddsRenderSurface->ReleaseDC(hDC);
		}
		pddsRenderSurface->Release();
}
*/	

}


void PartPurple(LPDIRECT3DDEVICE3 pd3dDevice)
{
//	DrawBackground(pd3dDevice, "phantasm.bmp", 1.0);
  pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
  pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_ONE );

	int x;
	int y;
	static float power=1.0;
	static float alpha=0.0;
	static float alphamap=0.0;
	static float dist=200.0;

	power = sin(VBLframe*0.02);

	if (MIDASgetPosition()>0x1600)
	{
		alpha+=VBLdelta*0.03;
		if (alpha>1.0)
			alpha = 1.0;
		
		dist -= VBLdelta;
		if (dist<20.0)
			dist = 20.0;
	}
	if (MIDASgetPosition()>0x1700)
	{
		alphamap+=VBLdelta*0.04;
		if (alphamap>1.0)
			alphamap = 1.0;
	}

	if ((GetAsyncKeyState(VK_UP)&0xf000))
		power+=0.01;
	if ((GetAsyncKeyState(VK_DOWN)&0xf000))
		power-=0.01;

	if ((GetAsyncKeyState(VK_RIGHT)&0xf000))
		alpha+=0.01;
	if ((GetAsyncKeyState(VK_LEFT)&0xf000))
		alpha-=0.01;

	if (alpha<0)
		alpha = 0;
	if (alpha>1.0)
		alpha = 1.0;

	D3DTLVERTEX   line[2*40*40];

	float oi,oj;

	int r,a,n=0;

	pd3dDevice->SetTexture( 0, 0 );
	pd3dDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

	for(y=00;y<480;y+=20)
	{
		int y1,y2;

		
		y2 = y+30.0+10.0*sin((VBLframe+y)*0.1);

		D3DCOLOR c;

		n++;
		if (n&1)
			c = D3DRGBA(0.42*0.3,0.31*0.3,0.19*0.3,1);
		else
			c = D3DRGBA(0.35*0.3,0.19*0.3,0.07*0.3,1);

	  g_Mesh[0] = D3DTLVERTEX( D3DVECTOR( 0, y2, 0.5 ), 0.5, c, 0, 0, 1 );
		g_Mesh[1] = D3DTLVERTEX( D3DVECTOR( 0, y, 0.5 ), 0.5, c, 0, 0, 0 );
		g_Mesh[2] = D3DTLVERTEX( D3DVECTOR( 640, y2, 0.5 ), 0.5, c, 0, 1, 1 );
		g_Mesh[3] = D3DTLVERTEX( D3DVECTOR( 0, y, 0.5 ), 0.5, c, 0, 0, 0 );
		g_Mesh[4] = D3DTLVERTEX( D3DVECTOR( 640, y2, 0.5 ), 0.5, c, 0, 1, 1 );
		g_Mesh[5] = D3DTLVERTEX( D3DVECTOR( 640, y, 0.5 ), 0.5, c, 0, 1, 0 );

     pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX,
		                       g_Mesh, 6, 0 );

	}


for(n=00;n<4;n++)
{
	oi=oj=-100;
	for(y=00;y<40;y++)
		for(a=0;a<40;a++)
		{
			float i,j,r,z;

			r = 40.0+10.0*sin((VBLframe+a*40)*3.1415927*2.0/400.0)*cos((-VBLframe+a*5)*3.1415927*2.0/400.0);
			r += 10.0*sin((VBLframe+y*10)*3.1415927*2.0/400.0);

			j = -50+y*dist+power*r*sin((10*a+VBLframe)*3.1415927*2.0/400.0);
			i = (4.0+n)*r*cos((10*a+-VBLframe)*3.1415927*2.0/400.0);


			j += a*dist/40.0;

			D3DCOLOR c = D3DRGBA(alpha*(1.0-n/4.0),alpha*(1.0-n/4.0),alpha*(1.0-n/4.0),1);
			
			i += 2.0*(y-20)*sin(VBLframe*0.02);

			line[y*80+a*2+0] = D3DTLVERTEX( D3DVECTOR( 320+oi, oj, 0.5 ), 0.5, c, 0, 0.3, 0.3 );
			line[y*80+a*2+1] = D3DTLVERTEX( D3DVECTOR( 320+i, j, 0.5 ), 0.5, c, 0, 0.7, 0.7 );
			
			oi=i;
			oj=j;
		}


	pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("mygirl.bmp") );
//  pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
//  pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_SRCALPHA );
//	pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_INVSRCALPHA );

	if (n==0)
	for(y=1;y<40;y++)
		for(a=1;a<40;a++)
		{
			float a1=alphamap,z=40.0;
			float x1,y1,x2,y2;

			line[y*80+a*2+0];
		

		  g_Mesh[0] = line[(y-1)*80+(a-1)*2+1];
			g_Mesh[0].tu=(a)/z;
			g_Mesh[0].tv=(y)/z;
			g_Mesh[0].color = D3DRGBA(a1,a1,a1,1);

			g_Mesh[1] = line[(y-1)*80+(a)*2+1];
			g_Mesh[1].tu=(a+1)/z;
			g_Mesh[1].tv=(y)/z;
			g_Mesh[1].color = D3DRGBA(a1,a1,a1,1);

			g_Mesh[2] = line[(y)*80+(a-1)*2+1];
			g_Mesh[2].tu=(a)/z;
			g_Mesh[2].tv=(y+1)/z;
			g_Mesh[2].color = D3DRGBA(a1,a1,a1,1);

		  g_Mesh[3] = line[(y)*80+(a-1)*2+1];
			g_Mesh[3].tu=(a)/z;
			g_Mesh[3].tv=(y+1)/z;
			g_Mesh[3].color = D3DRGBA(a1,a1,a1,1);

			g_Mesh[4] = line[(y)*80+(a)*2+1];
			g_Mesh[4].tu=(a+1)/z;
			g_Mesh[4].tv=(y+1)/z;
			g_Mesh[4].color = D3DRGBA(a1,a1,a1,1);

			g_Mesh[5] = line[(y-1)*80+(a)*2+1];
			g_Mesh[5].tu=(a+1)/z;
			g_Mesh[5].tv=(y)/z;
			g_Mesh[5].color = D3DRGBA(a1,a1,a1,1);

      pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX,g_Mesh, 6, 0 );
		}

	  pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
		pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("light.bmp") );
		pd3dDevice->DrawPrimitive( D3DPT_LINELIST, D3DFVF_TLVERTEX, line, 2*40*40, NULL );
}
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
}


//-----------------------------------------------------------------------------
// Name: App_Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT App_Render( LPDIRECT3DDEVICE3 pd3dDevice, 
                    LPDIRECT3DVIEWPORT3 pvViewport, D3DRECT* prcViewportRect )
{
    pvViewport->Clear2( 1UL, prcViewportRect, D3DCLEAR_TARGET,
                            0x00000000, 1.0f, 0L );

    // Begin the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
/*
 		Rotozoom(pd3dDevice,map[currentMap%5],100.0*sin(RAD(0.3*VBLframe)),zoom,0,0,prcViewportRect,1.0);
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_INVSRCALPHA );

		pd3dDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);
		drawRubber(pd3dDevice, VBLframe, 0.3);
		pd3dDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);
		drawRubber(pd3dDevice, VBLframe, 0.5);
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ONE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_ONE );
		DrawAddSprite(pd3dDevice, "mygirl1.bmp", 384, -32, 640, 224.0,a1,a2);
		DrawAddSprite(pd3dDevice, "mygirl2.bmp", 384, 224.0, 640, 480,a1,a2);

*/

//	if( (MIDASgetPosition()>>8)==0)
//		MIDASsetPosition(playHandle,0x18);		

	if( (MIDASgetPosition()>=0x0000) && (MIDASgetPosition()<0x0500) )
		PartBegin(pd3dDevice);
	if( (MIDASgetPosition()>=0x0500) && (MIDASgetPosition()<0x0a00) )
		PartMyGirl(pd3dDevice);
	if( (MIDASgetPosition()>=0x0a00) && (MIDASgetPosition()<0x1500) )
		PartFire(pd3dDevice);
	if( (MIDASgetPosition()>=0x1500) && (MIDASgetPosition()<0x1900) )
		PartPurple(pd3dDevice);

//	if( (MIDASgetPosition()>=0x0600) && (MIDASgetPosition()<0x1f00) )

	if( (MIDASgetPosition()>=0x1900) && (MIDASgetPosition()<0x1f00) )
		PartPhantasm(pd3dDevice);

	if( (MIDASgetPosition()>=0x2100) && (MIDASgetPosition()<0x3000) )
		PartScroll(pd3dDevice);




//	PartPhantasm(pd3dDevice);


//		if (VBLsync==1)
//	    RenderParticles( pd3dDevice );

		// End the scene.
		pd3dDevice->EndScene();
    }


  return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderParticles()
// Desc: Draws the system of particles
//-----------------------------------------------------------------------------
HRESULT RenderParticles( LPDIRECT3DDEVICE3 pd3dDevice )
{
  D3DMATERIAL mtrl;
	D3DUtil_InitMaterial( mtrl );
  D3DVECTOR vNorm = D3DVECTOR( 0.0f, 0.0f, 1.0f );

	// Turn on alpha blending for the particles
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetTexture( 0, D3DTextr_GetTexture("light.bmp") );

    // Do the particles
    for( float i=0; i<NUM_PARTICLES; i++ )
    {
			float x,y,z,r,a;
			r = 40+2*i;
			a = 10*VBLframe+i*(5.0*sin(RAD(3.0*VBLframe)));  //8.0;
			x = 320.0+ r*sin(RAD(a));
			y = 240.0+ r*cos(RAD(a));
			z = 25.0+10.0*sin(RAD(5.0*i+VBLframe));
			D3DCOLOR c;
			c = D3DRGBA(
								0.5+0.5*sin(RAD(0.21*VBLframe+4.98*i)),
								0.5+0.5*sin(RAD(3.12*VBLframe+1.28*i)),
								0.5+0.5*sin(RAD(7.94*VBLframe+2.17*i)),
								1
								);
		  g_Mesh[0] = D3DTLVERTEX( D3DVECTOR( x-z, y+z, 0.5 ), 0.5, c, 0, 0, 1 );
			g_Mesh[1] = D3DTLVERTEX( D3DVECTOR( x-z, y-z, 0.5 ), 0.5, c, 0, 0, 0 );
			g_Mesh[2] = D3DTLVERTEX( D3DVECTOR( x+z, y+z, 0.5 ), 0.5, c, 0, 1, 1 );
			g_Mesh[4] = D3DTLVERTEX( D3DVECTOR( x-z, y-z, 0.5 ), 0.5, c, 0, 0, 0 );
			g_Mesh[3] = D3DTLVERTEX( D3DVECTOR( x+z, y+z, 0.5 ), 0.5, c, 0, 1, 1 );
			g_Mesh[5] = D3DTLVERTEX( D3DVECTOR( x+z, y-z, 0.5 ), 0.5, c, 0, 1, 0 );

      pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX,
			                       g_Mesh, 6, 0 );

    }
    pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_InitDeviceObjects()
// Desc: Initialize scene objects
//-----------------------------------------------------------------------------
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice,
                               LPDIRECT3DVIEWPORT3 pvViewport )
{
    // Check parameters
    if( NULL==pd3dDevice || NULL==pvViewport )
        return E_INVALIDARG;

    D3DTextr_RestoreAllTextures( pd3dDevice );

	// Set up the dimensions for the background image
	D3DVIEWPORT2 vp;
	vp.dwSize = sizeof(vp);
	pvViewport->GetViewport2(&vp);
	
	// Get a ptr to the ID3D object to create materials and/or lights. Note:
	// the Release() call just serves to decrease the ref count.
    LPDIRECT3D3 pD3D;
    if( FAILED( pd3dDevice->GetDirect3D( &pD3D ) ) )
        return E_FAIL;
    pD3D->Release();

    //Create the render material
    if( FAILED( pD3D->CreateMaterial( &g_pmtrlRenderMtrl, NULL) ) )
        return E_FAIL;

	D3DMATERIAL       mtrl;
    D3DMATERIALHANDLE hmtrl;
    D3DUtil_InitMaterial( mtrl );
    g_pmtrlRenderMtrl->SetMaterial( &mtrl );
    g_pmtrlRenderMtrl->GetHandle( pd3dDevice, &hmtrl );
    pd3dDevice->SetLightState( D3DLIGHTSTATE_MATERIAL, hmtrl );

    // Set the transform matrices
    D3DVECTOR vEyePt    = D3DVECTOR( 0.0f, 0.0f, -20.0f );
    D3DVECTOR vLookatPt = D3DVECTOR( 0.0f, 0.0f,   0.0f );
    D3DVECTOR vUpVec    = D3DVECTOR( 0.0f, 0.0f,   1.0f );
    D3DMATRIX matView, matProj;

    D3DUtil_SetViewMatrix( matView, vEyePt, vLookatPt, vUpVec );
  	D3DUtil_SetProjectionMatrix( matProj, 1.57f, 1.0f, 1.0f, 100.0f );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_VIEW,       &matView );
    pd3dDevice->SetTransform( D3DTRANSFORMSTATE_PROJECTION, &matProj );

    // Set any appropiate state
    pd3dDevice->SetLightState(  D3DLIGHTSTATE_AMBIENT,     0xffffffff );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_ONE );
    pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_ONE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_DITHERENABLE, TRUE );
	pd3dDevice->SetRenderState( D3DRENDERSTATE_SPECULARENABLE, FALSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
	pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT App_FinalCleanup( LPDIRECT3DDEVICE3 pd3dDevice, 
                          LPDIRECT3DVIEWPORT3 pvViewport )
{
    App_DeleteDeviceObjects( pd3dDevice, pvViewport );

    D3DTextr_DestroyTexture( "light.bmp" );
    D3DTextr_DestroyTexture( "mygirl.bmp" );
    D3DTextr_DestroyTexture( "phantasm.bmp" );
    D3DTextr_DestroyTexture( "knights.bmp" );
    D3DTextr_DestroyTexture( "title.bmp" );
    D3DTextr_DestroyTexture( "anim.bmp" );
    D3DTextr_DestroyTexture( "flare.bmp" );
    D3DTextr_DestroyTexture( "violet.bmp" );
    D3DTextr_DestroyTexture( "scroll.bmp" );
    D3DTextr_DestroyTexture( "line.bmp" );
    D3DTextr_DestroyTexture( "pub.bmp" );
    D3DTextr_DestroyTexture( "back.bmp" );
//    D3DTextr_DestroyTexture( ".bmp" );


		#ifdef MUSIC
    /* Uninitialization */

    /* Remove music sync callback: */
    if ( !MIDASsetMusicSyncCallback(playHandle, NULL) )
        MIDASerror();
    
    /* Stop playing module: */
    if ( !MIDASstopModule(playHandle) )
        MIDASerror();

		MIDASstopBackgroundPlay();

    /* Deallocate the module: */
    if ( !MIDASfreeModule(module) )
        MIDASerror();

    /* Close MIDAS: */
    if ( !MIDASclose() )
        MIDASerror();
		#endif
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_DeleteDeviceObjects()
// Desc: Called when the app is exitting, or the device is being changed,
//       this function deletes any device dependant objects.
//-----------------------------------------------------------------------------
VOID App_DeleteDeviceObjects( LPDIRECT3DDEVICE3 pd3dDevice, 
                              LPDIRECT3DVIEWPORT3 pvViewport )
{
    D3DTextr_InvalidateAllTextures();

    SAFE_RELEASE( g_pmtrlRenderMtrl );
}




//----------------------------------------------------------------------------
// Name: App_RestoreSurfaces
// Desc: Restores any previously lost surfaces. Must do this for all surfaces
//       (including textures) that the app created.
//----------------------------------------------------------------------------
HRESULT App_RestoreSurfaces()
{
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: App_ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT App_ConfirmDevice( DDCAPS* pddDriverCaps,
						   D3DDEVICEDESC* pd3dDeviceDesc )
{
    // Get triangle caps (Hardware or software) and check for alpha blending
//    LPD3DPRIMCAPS pdpc = &pd3dDeviceDesc->dpcTriCaps;

//    if( 0 == ( pdpc->dwSrcBlendCaps & pdpc->dwDestBlendCaps & D3DBLEND_ONE ) )
//        return E_FAIL;

    return S_OK;
}





