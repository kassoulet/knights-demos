//-----------------------------------------------------------------------------
// File: WinMain.cpp
//
// Desc: Windows code for Direct3D programs
//
//			 This code uses the Direct3D program framework.
//
//
// Copyright (c) 1996-1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#define STRICT


#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include "D3DFrame.h"
#include "D3DEnum.h"
#include "D3DUtil.h"
#include "resource.h"


void WriteTGA640( LPDIRECT3DDEVICE3 pd3dDevice);
void WriteTGA( LPDIRECT3DDEVICE3 pd3dDevice);

bool screenshot=false;

//-----------------------------------------------------------------------------
// Global variables for using the D3D program framework class
//-----------------------------------------------------------------------------
CD3DFramework* g_pFramework 			 = NULL;
BOOL					 g_bActive					 = FALSE;
BOOL					 g_bReady 					 = FALSE;
BOOL					 g_bShowStats 			 = FALSE;
BOOL					 g_bFrameMoving 		 = TRUE;
BOOL					 g_bSingleStep			 = FALSE;
BOOL					 g_bWindowed				 = FALSE;
RECT					 g_rcWindow;
extern BOOL 	 g_bAppUseZBuffer;
extern BOOL 	 g_bAppUseBackBuffer;
extern TCHAR*  g_strAppTitle;

enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHTOSOFTWARE };




float VBLframe,VBLdelta;

//-----------------------------------------------------------------------------
// Local function-prototypes
//-----------------------------------------------------------------------------
INT 		CALLBACK AboutProc( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
HRESULT Initialize3DEnvironment( HWND );
HRESULT Change3DEnvironment( HWND );
HRESULT Render3DEnvironment();
VOID		Cleanup3DEnvironment();
VOID		DisplayFrameworkError( HRESULT, APPMSGTYPE );
VOID		AppShowStats();
VOID		AppOutputText( LPDIRECT3DDEVICE3, DWORD, DWORD, CHAR* );
VOID		AppPause( BOOL );

void		keybAction(int x, int y);


//-----------------------------------------------------------------------------
// External function-prototypes
//-----------------------------------------------------------------------------
HRESULT App_ConfirmDevice( DDCAPS*, D3DDEVICEDESC* );
HRESULT App_OneTimeSceneInit( HWND );
VOID		App_DeleteDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
HRESULT App_InitDeviceObjects( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );
HRESULT App_FrameMove( LPDIRECT3DDEVICE3, FLOAT );
HRESULT App_Render( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3, D3DRECT* );
HRESULT App_RestoreSurfaces();
HRESULT App_FinalCleanup( LPDIRECT3DDEVICE3, LPDIRECT3DVIEWPORT3 );


void BURST_Start();
void BURST_LoadMenu();
void BURST_Menu(IDirect3DViewport3 * pvViewport,IDirect3DDevice3 * _pd3dDevice, D3DRECT * rect);


// This is the winproc of the spash window
LRESULT CALLBACK SplashProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_CLOSE:
			DestroyWindow( hWnd );
			return 0;
				
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0L;

		case WM_KEYUP:
			SendMessage( hWnd, WM_CLOSE, 0, 0 );
			break;
		}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

HWND hBack,hText,hWnd,hIcon,hIcon2;

// This display the splash window
void ShowSplash( HINSTANCE hInst)
{
	// Register the window class
	WNDCLASS wndClass = { 0, SplashProc, 0, 0, hInst,
												0,
												LoadCursor(NULL, IDC_ARROW), 
												(HBRUSH)GetStockObject(BLACK_BRUSH), 
												0,
												"Splash" };
	RegisterClass( &wndClass );

	// Create our main window
	int width =512;
	int height=128+32;
	int x = (GetSystemMetrics(SM_CXSCREEN)-width)/1;
	int y = (GetSystemMetrics(SM_CYSCREEN)-height)/2;

	hWnd	= CreateWindowEx(WS_EX_TOPMOST , "Splash", "Splash",
														WS_POPUP, x,0,
														width, height, 0L, 0L, hInst, 0L );
/*
	hIcon = CreateWindow( "STATIC", "title.bmp",
														WS_VISIBLE | WS_CHILD | SS_BITMAP, 0,0,
														32, 32, hWnd, 0L, hInst, 0L );
*/
	hIcon2 = CreateWindow( "STATIC", "pub.bmp",
														WS_VISIBLE | WS_CHILD | SS_BITMAP, 256,0,
														32, 32, hWnd, 0L, hInst, 0L );

	ShowWindow( hWnd, SW_SHOWNORMAL );
	UpdateWindow( hWnd );
}

// return true if the given option is the the command line
bool MatchOption(char * cmd,char option)
{
	char* param;

	option = tolower(option);

	if (param=strchr(cmd,'-'))
	{
		if (tolower(*param+1)==option)
			return true;
	}
	if (param=strchr(cmd,'/'))
	{
		if (tolower(*(param+1))==option)
			return true;
	}
	return false;
}

// the winproc of the preview window
LRESULT CALLBACK PreviewProc( HWND hwin, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_ERASEBKGND:
		return 0;

		case WM_PAINT:
		{
			if (!GetUpdateRect(hwin,NULL,FALSE))
				return 0;
			PAINTSTRUCT paint;

			BeginPaint(hwin,&paint);
			HDC dc = paint.hdc;

			RECT r;
			int w,h;

			GetWindowRect(hwin,&r);
			w = r.right-r.left;
			h = r.bottom-r.top;

			HDC dcBitmap;

			HBITMAP hbm;

			hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), "title.bmp",
			                       IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

			dcBitmap = CreateCompatibleDC(dc);
			SelectObject(dcBitmap, hbm);
			
			int ry,rh;
			rh = w/2;
			ry = (h-rh)/2;

			StretchBlt(dc,0,ry,w,rh,dcBitmap,0,0,512,256,SRCCOPY);

			EndPaint(hWnd,&paint);
		return 0;
		}

		case WM_CLOSE:
			DestroyWindow( hwin );
			return 0;
				
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0L;

//		case WM_KEYUP:
//			SendMessage( hwin, WM_CLOSE, 0, 0 );
//			break;
		}

	return DefWindowProc( hwin, uMsg, wParam, lParam );
}

// display the preview window
void ShowPreview( HWND hwin, HINSTANCE hInst)
{
	// Register the window class
	WNDCLASS wndClass = { 0, PreviewProc, 0, 0, hInst,
												0,
												LoadCursor(NULL, IDC_ARROW), 
												(HBRUSH)GetStockObject(BLACK_BRUSH) , 
												0,
												"Preview" };
	RegisterClass( &wndClass );

	HWND hWnd;
	// Create our main window
	RECT r;
	GetWindowRect(hwin,&r);

	hWnd	= CreateWindow("Preview", "Preview",
														WS_CHILD, 0,0,r.right-r.left,r.bottom-r.top, hwin, 0, hInst, 0 );

	ShowWindow( hWnd, SW_SHOWNORMAL );
	UpdateWindow( hWnd );


	MSG msg;
	while(GetMessage(&msg, 0, 0, 0) == TRUE)
	{
		TranslateMessage(&msg); 
		DispatchMessage(&msg);
	}
}


void ShowConfig(HINSTANCE hInst)
{
	DialogBox( hInst,MAKEINTRESOURCE(IDD_ABOUT), NULL, AboutProc);
}

int ScreenSaver(HINSTANCE hInst)
{
	// Register the window class
		WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInst,
													LoadIcon( hInst, MAKEINTRESOURCE(IDI_MAIN_ICON)),
													LoadCursor(NULL, IDC_ARROW), 
													(HBRUSH)GetStockObject(BLACK_BRUSH),
													NULL,
													TEXT("Render Window") };
		RegisterClass( &wndClass );

		// Create our main window

		HWND hWnd = CreateWindow( TEXT("Render Window"), g_strAppTitle,
															WS_POPUP, 0,0,
															GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CXSCREEN), 0L, 0L, hInst, 0L );

		ShowWindow( hWnd, SW_SHOWNORMAL );
		UpdateWindow( hWnd );

		// Save the window size/pos for switching modes
		GetWindowRect( hWnd, &g_rcWindow );

		// Load keyboard accelerators
		HACCEL hAccel = LoadAccelerators( hInst, MAKEINTRESOURCE(IDR_MAIN_ACCEL) );

	// Enumerate available D3D devices, passing a callback that allows devices
	// to be accepted/rejected based on what capabilities the app requires.
	HRESULT hr;
	if( FAILED( hr = D3DEnum_EnumerateDevices( App_ConfirmDevice ) ) )
	{
		DisplayFrameworkError( hr, MSGERR_APPMUSTEXIT );
				return 0;
	}
	if (D3DEnum_SelectDefaultDriver(D3DENUM_FULLSCREENONLY)!=S_OK)
	{
		// Display Device Control
		{
			AppPause(TRUE);
			if( g_bWindowed )
				GetWindowRect( hWnd, &g_rcWindow );
			if( IDOK == D3DEnum_UserDlgSelectDriver( hWnd, g_bWindowed ) )
			{
				D3DEnum_DriverInfo* pDriverInfo;
				D3DEnum_DeviceInfo* pDeviceInfo;
				D3DEnum_GetSelectedDriver( &pDriverInfo, &pDeviceInfo );
				g_bWindowed = pDeviceInfo->bWindowed;
				Change3DEnvironment( hWnd );
			}
			AppPause(FALSE);
		}
	}	

	// Check if we could not get a device that renders into a window, which
	// means the display must be 16- or 256-color mode. If so, let's bail.
	D3DEnum_DriverInfo* pDriverInfo;
	D3DEnum_DeviceInfo* pDeviceInfo;
	D3DEnum_GetSelectedDriver( &pDriverInfo, &pDeviceInfo );

	// Initialize the 3D environment for the app
		if( FAILED( hr = Initialize3DEnvironment( hWnd ) ) )
		{
			Cleanup3DEnvironment();
		DisplayFrameworkError( hr, MSGERR_APPMUSTEXIT );
				return 0;
		}

		// Now we're ready to recieve and process Windows messages.
	BOOL bGotMsg;
	MSG  msg;
	PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );
	g_bReady = TRUE;


		while( WM_QUIT != msg.message  )
		{
		// Use PeekMessage() if the app is active, so we can use idle time to
		// render the scene. Else, use GetMessage() to avoid eating CPU time.
		if( g_bActive )
			bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
		else
			bGotMsg = GetMessage( &msg, NULL, 0U, 0U );

		if( bGotMsg )
				{
			// Translate and dispatch the message
						if( 0 == TranslateAccelerator( hWnd, hAccel, &msg ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
				}
		else
		{
			// Render a frame during idle time (no messages are waiting)
			if( g_bActive && g_bReady )
			{
				if( FAILED( Render3DEnvironment() ) )
				{
					MessageBox( NULL, TEXT("An error occurred during rendering."
										"\n\nThis program will now exit"),
								g_strAppTitle, MB_ICONERROR|MB_OK );
					DestroyWindow( hWnd );
				}
			}
		}
		}

	return 0;
}

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//			 message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
/*
	if (MatchOption(strCmdLine,'p'))
	{
		// We are called to display the preview window
		char * p;
		HWND hwin;
		p = strrchr(strCmdLine,' ');
		p++;
		hwin = (HWND)atol(p);
		ShowPreview(hwin,hInst);
		return 0;
	}	

	if (MatchOption(strCmdLine,'s'))
*/
	{
		// We are called to kick some ass with the screen saver !
		ShowSplash(hInst);
		ScreenSaver(hInst);
		return 0;
	}

/*
//	if (MatchOption(strCmdLine,'c'))
	{
		// We are called to show the configuration window
		ShowConfig(hInst);
		return 0;
	}
*/
return 0;
}




//-----------------------------------------------------------------------------
// Name: WndProc()
// Desc: This is the basic Windows-programming function that processes
//			 Windows messages. We need to handle window movement, painting,
//			 and destruction.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static int x,y,ox=-1,oy=-1;

	switch( uMsg )
	{
		case WM_PAINT:
			if( g_pFramework )
			{
				// If we are paused, and in fullscreen mode, give the dialogs
				// a GDI surface to draw on.
				if( !g_bReady && !g_bWindowed)
					g_pFramework->FlipToGDISurface( TRUE );
				else // Simply repaint the frame's contents
					g_pFramework->ShowFrame();
			}
			break;

				case WM_MOVE:
						if( g_bActive && g_bReady && g_bWindowed )
			{
					GetWindowRect( hWnd, &g_rcWindow );
								g_pFramework->Move( (SHORT)LOWORD(lParam), (SHORT)HIWORD(lParam) );
			}
						break;

				case WM_SIZE:
						// Check to see if we are losing our window...
						if( SIZE_MAXHIDE==wParam || SIZE_MINIMIZED==wParam )
								g_bActive = FALSE;
			else g_bActive = TRUE;

						// A new window size will require a new viewport and backbuffer
						// size, so the 3D structures must be changed accordingly.
						if( g_bActive && g_bReady && g_bWindowed )
			{
				g_bReady = FALSE;
				GetWindowRect( hWnd, &g_rcWindow );
				Change3DEnvironment( hWnd );
				g_bReady = TRUE;
			}
						break;

		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
			break;

				case WM_SETCURSOR:
						if( g_bActive && g_bReady && (!g_bWindowed) )
						{
								SetCursor(NULL);
								return TRUE;
						}
						break;

				case WM_CLOSE:
						DestroyWindow( hWnd );
						return 0;
				
				case WM_DESTROY:
						Cleanup3DEnvironment();
						PostQuitMessage(0);
						return 0L;

#ifndef EXE_FILE
/*
		case WM_MOUSEMOVE:
				x = LOWORD(lParam);  // horizontal position of cursor 
				y = HIWORD(lParam);  // vertical position of cursor 
				if (ox!=-1)
					if ((abs(x-ox)+abs(y-oy)) > 10)
						SendMessage( hWnd, WM_CLOSE, 0, 0 );
				ox=x;
				oy=y;
			break;
*/
#endif
		case WM_KEYDOWN:
			if (wParam == VK_TAB)
			{
				g_bShowStats=TRUE;
			}
			break;
		case WM_KEYUP:
			if (wParam == VK_TAB)
			{
				g_bShowStats=FALSE;
			}
			if (wParam == VK_ESCAPE)
			{
				SendMessage( hWnd, WM_CLOSE, 0, 0 );
			}
			if (wParam == VK_SPACE)
			{
				screenshot=true;
			}
			else	
			if (wParam == VK_F1)
			{
				// Display the driver-selection dialog box.
				if( g_bActive && g_bReady )
				{
					AppPause(TRUE);
					if( g_bWindowed )
						GetWindowRect( hWnd, &g_rcWindow );
					if( IDOK == D3DEnum_UserDlgSelectDriver( hWnd, g_bWindowed ) )
					{
						D3DEnum_DriverInfo* pDriverInfo;
						D3DEnum_DeviceInfo* pDeviceInfo;
						D3DEnum_GetSelectedDriver( &pDriverInfo, &pDeviceInfo );
						g_bWindowed = pDeviceInfo->bWindowed;
						Change3DEnvironment( hWnd );
					}
					AppPause(FALSE);
				}
			}
//#ifndef EXE_FILE
//			else
//				SendMessage( hWnd, WM_CLOSE, 0, 0 );
//#endif
			break;


			case WM_COMMAND:
				switch( LOWORD(wParam) )
				{
					case SC_MONITORPOWER:
						// Prevent potential crashes when the monitor powers down
						return 1;
				}
				break;

		}

		return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
						



//-----------------------------------------------------------------------------
// Name: AboutProc()
// Desc: Minimal message proc function for the about box
//-----------------------------------------------------------------------------
BOOL CALLBACK AboutProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM )
{
		if( WM_COMMAND == uMsg )
		if( IDOK == LOWORD(wParam) || IDCANCEL == LOWORD(wParam) )
			EndDialog (hWnd, TRUE);

		return ( WM_INITDIALOG == uMsg ) ? TRUE : FALSE;
}




//-----------------------------------------------------------------------------
// Note: From this point on, the code is DirectX specific support for the app.
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Name: AppInitialize()
// Desc: Initializes the program framework, then calls the app-specific function
//			 to initialize device specific objects. This code is structured to
//			 handled any errors that may occur duing initialization
//-----------------------------------------------------------------------------
HRESULT AppInitialize( HWND hWnd )
{
	D3DEnum_DriverInfo* pDriverInfo;
	D3DEnum_DeviceInfo* pDeviceInfo;
		DWORD 	dwFrameworkFlags = 0L;
	HRESULT hr;

		D3DEnum_GetSelectedDriver( &pDriverInfo, &pDeviceInfo );

		dwFrameworkFlags |= (!g_bWindowed 				? D3DFW_FULLSCREEN : 0L );
		dwFrameworkFlags |= ( g_bAppUseZBuffer		? D3DFW_ZBUFFER 	 : 0L );
		dwFrameworkFlags |= ( g_bAppUseBackBuffer ? D3DFW_BACKBUFFER : 0L );

	// Initialize the D3D framework
		if( SUCCEEDED( hr = g_pFramework->Initialize( hWnd, &pDriverInfo->guid,
									 &pDeviceInfo->guid, &pDeviceInfo->pCurrentMode->ddsd, 
									 dwFrameworkFlags ) ) )
	{
		// Let the app run its startup code which creates the 3d scene.
		if( SUCCEEDED( hr = App_InitDeviceObjects( g_pFramework->GetD3DDevice(),
																					g_pFramework->GetViewport() ) ) )
			return S_OK;
		else
		{
			App_DeleteDeviceObjects( g_pFramework->GetD3DDevice(),
																 g_pFramework->GetViewport() );
			g_pFramework->DestroyObjects();
		}
	}

	// If we get here, the first initialization passed failed. If that was with a
	// hardware device, try again using a software rasterizer instead.
	if( pDeviceInfo->bIsHardware )
	{
		// Try again with a software rasterizer
		DisplayFrameworkError( hr, MSGWARN_SWITCHTOSOFTWARE );
		D3DEnum_SelectDefaultDriver( D3DENUM_SOFTWAREONLY );
		return AppInitialize( hWnd );
	}

	return hr;
}




//-----------------------------------------------------------------------------
// Name: Initialize3DEnvironment()
// Desc: Called when the app window is initially created, this triggers
//			 creation of the remaining portion (the 3D stuff) of the app.
//-----------------------------------------------------------------------------
HRESULT Initialize3DEnvironment( HWND hWnd )
{
	HRESULT hr;

	// Initialize the app
	if( FAILED( hr = App_OneTimeSceneInit( hWnd ) ) )
		return E_FAIL;

		// Create a new CD3DFramework class. This class does all of our D3D
		// initialization and manages the common D3D objects.
		if( NULL == ( g_pFramework = new CD3DFramework() ) )
		return E_OUTOFMEMORY;

	// Finally, initialize the framework and scene.
	return AppInitialize( hWnd );
}

		


//-----------------------------------------------------------------------------
// Name: Change3DEnvironment()
// Desc: Handles driver, device, and/or mode changes for the app.
//-----------------------------------------------------------------------------
HRESULT Change3DEnvironment( HWND hWnd )
{
	HRESULT hr;
		
		// Release all objects that need to be re-created for the new device
		App_DeleteDeviceObjects( g_pFramework->GetD3DDevice(), 
														 g_pFramework->GetViewport() );

	// Release the current framework objects (they will be recreated later on)
	if( FAILED( hr = g_pFramework->DestroyObjects() ) )
	{
		DisplayFrameworkError( hr, MSGERR_APPMUSTEXIT );
				DestroyWindow( hWnd );
		return hr;
	}

	// In case we're coming from a fullscreen mode, restore the window size
	if( g_bWindowed )
	{
		SetWindowPos( hWnd, HWND_NOTOPMOST, g_rcWindow.left, g_rcWindow.top,
											( g_rcWindow.right - g_rcWindow.left ), 
											( g_rcWindow.bottom - g_rcWindow.top ), SWP_SHOWWINDOW );
	}

		// Inform the framework class of the driver change. It will internally
		// re-create valid surfaces, a d3ddevice, and a viewport.
	if( FAILED( hr = AppInitialize( hWnd ) ) )
	{
		DisplayFrameworkError( hr, MSGERR_APPMUSTEXIT );
		DestroyWindow( hWnd );
		return hr;
	}

	// Trigger the rendering of a frame and return
	g_bSingleStep = TRUE;
	return S_OK;
}


void BurstLoop(LPDIRECT3DDEVICE3 pd3dDevice,LPDIRECT3DVIEWPORT3 pvViewport, D3DRECT* prcViewRect);


//-----------------------------------------------------------------------------
// Name: Render3DEnvironment()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
HRESULT Render3DEnvironment()
{
	// Check the cooperative level before rendering
	if( FAILED( g_pFramework->GetDirectDraw()->TestCooperativeLevel() ) )
		return S_OK;

	// Get the current time
	VBLframe = timeGetTime() * 0.001;
	static float oldFrame;
	VBLdelta = VBLframe-oldFrame;
	oldFrame = VBLframe;


	// FrameMove (animate) the scene
	if( g_bFrameMoving || g_bSingleStep )
	{
		if( FAILED( App_FrameMove( g_pFramework->GetD3DDevice(), VBLframe ) ) )
			return E_FAIL;

		g_bSingleStep = FALSE;
	}

		//Render the scene
		if( FAILED( App_Render( g_pFramework->GetD3DDevice(),
												g_pFramework->GetViewport(), 
														(D3DRECT*)g_pFramework->GetViewportRect() ) ) )
		return E_FAIL;

/*
	BurstLoop( g_pFramework->GetD3DDevice(),g_pFramework->GetViewport(), 
														(D3DRECT*)g_pFramework->GetViewportRect());
*/		

	if (screenshot)
	{
		WriteTGA640(g_pFramework->GetD3DDevice());
		screenshot=false;
	}

//	WriteTGA(g_pFramework->GetD3DDevice());


	// Show the frame rate, etc.
	if( g_bShowStats )
		AppShowStats();

		// Show the frame on the primary surface.
		if( DDERR_SURFACELOST == g_pFramework->ShowFrame() )
		{
		g_pFramework->RestoreSurfaces();
		App_RestoreSurfaces();
	}

		return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup3DEnvironment()
// Desc: Cleanup scene objects
//-----------------------------------------------------------------------------
VOID Cleanup3DEnvironment()
{
		if( g_pFramework )
		{
				App_FinalCleanup( g_pFramework->GetD3DDevice(), 
													g_pFramework->GetViewport() );

				SAFE_DELETE( g_pFramework );
		}
	g_bActive = FALSE;
	g_bReady	= FALSE;
}


	

//-----------------------------------------------------------------------------
// Name: AppPause()
// Desc: Called in to toggle the pause state of the app. This function
//			 brings the GDI surface to the front of the display, so drawing
//			 output like message boxes and menus may be displayed.
//-----------------------------------------------------------------------------
VOID AppPause( BOOL bPause )
{
		static DWORD dwAppPausedCount = 0L;

		if( bPause && 0 == dwAppPausedCount )
				if( g_pFramework )
						g_pFramework->FlipToGDISurface( TRUE );

		dwAppPausedCount += ( bPause ? +1 : -1 );

		g_bReady = (0==dwAppPausedCount);
}




//-----------------------------------------------------------------------------
// Name: AppShowStats()
// Desc: Shows frame rate and dimensions of the rendering device. Note: a 
//			 "real" app wouldn't query the surface dimensions each frame.
//-----------------------------------------------------------------------------

VOID AppShowStats()
{
		static FLOAT fFPS 		 = 0.0f;
		static FLOAT fLastTime = 0.0f;
		static DWORD dwFrames  = 0L;

	// Keep track of the time lapse and frame count
	FLOAT fTime = timeGetTime() * 0.001f; // Get current time in seconds
	++dwFrames;

	// Update the frame rate once per second
	if( fTime - fLastTime > 1.0f )
		{
				fFPS			= dwFrames / (fTime - fLastTime);
				fLastTime = fTime;
				dwFrames	= 0L;
		}

		// Get dimensions of the render surface 
		DDSURFACEDESC2 ddsd;
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		g_pFramework->GetRenderSurface()->GetSurfaceDesc(&ddsd);

		// Setup the text buffer to write out
		CHAR buffer[80];
		sprintf( buffer, "%7.02f fps", fFPS);
		AppOutputText( g_pFramework->GetD3DDevice(), 0, 0, buffer );
		sprintf( buffer, "Delta %7.02f ", VBLdelta);
//		AppOutputText( g_pFramework->GetD3DDevice(), 0, 10, buffer );
}




//-----------------------------------------------------------------------------
// Name: AppOutputText()
// Desc: Draws text on the window.
//-----------------------------------------------------------------------------
VOID AppOutputText( LPDIRECT3DDEVICE3 pd3dDevice, DWORD x, DWORD y, CHAR* str )
{
		LPDIRECTDRAWSURFACE4 pddsRenderSurface;
		if( FAILED( pd3dDevice->GetRenderTarget( &pddsRenderSurface ) ) )
				return;

		// Get a DC for the surface. Then, write out the buffer
		HDC hDC;
		if( SUCCEEDED( pddsRenderSurface->GetDC(&hDC) ) )
		{
				SetTextColor( hDC, RGB(255,255,0) );
				SetBkMode( hDC, TRANSPARENT );
				ExtTextOut( hDC, x, y, 0, NULL, str, strlen(str), NULL );
		
				pddsRenderSurface->ReleaseDC(hDC);
		}
		pddsRenderSurface->Release();
}

typedef unsigned short word;

void WriteTGA( LPDIRECT3DDEVICE3 pd3dDevice)
{
	static framecount=0;
	LPDIRECTDRAWSURFACE4 pddsRenderSurface;
	if( FAILED( pd3dDevice->GetRenderTarget( &pddsRenderSurface ) ) )
			return;

	char str[256];
	sprintf(str,"Cochlea - Stream - %08i.tga",framecount++);

	FILE * f = fopen(str,"wb");
  fputc(0, f);                          /* id length (no id saved) */
  fputc(0, f);       /* palette type */
  fputc(2, f);       /* image type */
  fputc(0, f);                         /* first colour */
  fputc(0, f);                         /* first colour */
  fputc(0, f);    /* number of colours */
  fputc(0, f);    /* number of colours */
  fputc(0, f);      /* palette entry size */
  fputc(0, f);                         /* left */
  fputc(0, f);                         /* left */
  fputc(0, f);                         /* top */
  fputc(0, f);                         /* top */
  fputc(80&255, f);                    /* width */
  fputc(80>>8, f);                    /* width */
  fputc(60&255, f);                    /* height */
  fputc(60>>8, f);                    /* height */
  fputc(24, f);                      /* bits per pixel */
  fputc(0, f);                          /* descriptor (bottom to top) */

	int x,y;


	DDSURFACEDESC2 desc;

	desc.dwSize=sizeof desc;
	HRESULT re = pddsRenderSurface->Lock(NULL, &desc, DDLOCK_WAIT, NULL);

	int c,r,g,b;

	c = desc.ddpfPixelFormat.dwRGBBitCount;
	r = desc.ddpfPixelFormat.dwRBitMask;
	g = desc.ddpfPixelFormat.dwGBitMask;
	b = desc.ddpfPixelFormat.dwBBitMask;

//	char str[256];
	if (re==DD_OK)
	{

		word * buffer = (word*) desc.lpSurface;
		int pitch	 = desc.lPitch;

		for(y=479;y>=0;y-=4*2)
		{
			word *s = buffer+pitch*(y)/2;

//		*(s+480)= 255;

//			char s[640*2];
//		fwrite(s,1,640*2,f);

			for(x=0;x<640;x+=4*2)
			{
				word c;

				c = *s;

				word r = c>>11;
				word g = (c>>6)&31;
			  fputc((c&0x1f)<<3, f); // b
			  fputc(g<<3, f); // g
			  fputc(r<<3, f);  // r

				s+=4*2;
			}

/*
			for(x=0;x<640;x++)
			{
				fputc(x/20, f);                         
				fputc(0x00, f);                         
			}
*/
		}


		pddsRenderSurface->Unlock(0);
	}

	fclose(f);
	pddsRenderSurface->Release();
}

void WriteTGA640( LPDIRECT3DDEVICE3 pd3dDevice)
{
	static framecount=0;

	LPDIRECTDRAWSURFACE4 pddsRenderSurface;
	if( FAILED( pd3dDevice->GetRenderTarget( &pddsRenderSurface ) ) )
			return;

	char str[256];
	sprintf(str,"Cochlea - ScreenShoot - %03i.tga",framecount++);

	FILE * f = fopen(str,"wb");
  fputc(0, f);                          /* id length (no id saved) */
  fputc(0, f);       /* palette type */
  fputc(2, f);       /* image type */
  fputc(0, f);                         /* first colour */
  fputc(0, f);                         /* first colour */
  fputc(0, f);    /* number of colours */
  fputc(0, f);    /* number of colours */
  fputc(0, f);      /* palette entry size */
  fputc(0, f);                         /* left */
  fputc(0, f);                         /* left */
  fputc(0, f);                         /* top */
  fputc(0, f);                         /* top */
  fputc(640&255, f);                    /* width */
  fputc(640>>8, f);                    /* width */
  fputc(480&255, f);                    /* height */
  fputc(480>>8, f);                    /* height */
  fputc(24, f);                      /* bits per pixel */
  fputc(0, f);                          /* descriptor (bottom to top) */

	int x,y;


	DDSURFACEDESC2 desc;

	desc.dwSize=sizeof desc;
	HRESULT re = pddsRenderSurface->Lock(NULL, &desc, DDLOCK_WAIT, NULL);

	int c,r,g,b;

	c = desc.ddpfPixelFormat.dwRGBBitCount;
	r = desc.ddpfPixelFormat.dwRBitMask;
	g = desc.ddpfPixelFormat.dwGBitMask;
	b = desc.ddpfPixelFormat.dwBBitMask;

	if (re==DD_OK)
	{

		word * buffer = (word*) desc.lpSurface;
		int pitch	 = desc.lPitch;

		for(y=479;y>=0;y--)
		{
			word *s = buffer+pitch*(y)/2;

//		*(s+480)= 255;

//			char s[640*2];
//		fwrite(s,1,640*2,f);

			for(x=0;x<640;x++)
			{
				word c;

				c = *s;

				word r = c>>11;
				word g = (c>>6)&31;
			  fputc((c&0x1f)<<3, f); // b
			  fputc(g<<3, f); // g
			  fputc(r<<3, f);  // r

				s++;
			}

/*
			for(x=0;x<640;x++)
			{
				fputc(x/20, f);                         
				fputc(0x00, f);                         
			}
*/
		}


		pddsRenderSurface->Unlock(0);
	}

	fclose(f);
	pddsRenderSurface->Release();
}


//-----------------------------------------------------------------------------
// Name: DisplayFrameworkError()
// Desc: Displays error messages in a message box
//-----------------------------------------------------------------------------
VOID DisplayFrameworkError( HRESULT hr, APPMSGTYPE errType )
{
	CHAR strMsg[512];

	switch( hr )
	{
		case D3DENUMERR_NOCOMPATIBLEDEVICES:
			strcpy( strMsg, TEXT("Could not find any compatible Direct3D\n"
						"devices.") );
			break;
		case D3DENUMERR_SUGGESTREFRAST:
			strcpy( strMsg, TEXT("Could not find any compatible devices.\n\n"
								"Try enabling the reference rasterizer using\n"
					"EnableRefRast.reg.") );
			break;
		case D3DENUMERR_ENUMERATIONFAILED:
			strcpy( strMsg, TEXT("Enumeration failed. Your system may be in an\n"
					"unstable state and need to be rebooted") );
			break;
		case D3DFWERR_INITIALIZATIONFAILED:
			strcpy( strMsg, TEXT("Generic initialization error.\n\nEnable "
						"debug output for detailed information.") );
			break;
		case D3DFWERR_NODIRECTDRAW:
			strcpy( strMsg, TEXT("No DirectDraw") );
			break;
		case D3DFWERR_NODIRECT3D:
			strcpy( strMsg, TEXT("No Direct3D") );
			break;
		case D3DFWERR_INVALIDMODE:
			strcpy( strMsg, TEXT("This program requires a 16-bit (or higher) "
							"display mode\nto run in a window.\n\nPlease switch "
						"your desktop settings accordingly.") );
			break;
		case D3DFWERR_COULDNTSETCOOPLEVEL:
			strcpy( strMsg, TEXT("Could not set Cooperative Level") );
			break;
		case D3DFWERR_NO3DDEVICE:
			strcpy( strMsg, TEXT("No 3D Device") );
			break;
		case D3DFWERR_NOZBUFFER:
			strcpy( strMsg, TEXT("No ZBuffer") );
			break;
		case D3DFWERR_NOVIEWPORT:
			strcpy( strMsg, TEXT("No Viewport") );
			break;
		case D3DFWERR_NOPRIMARY:
			strcpy( strMsg, TEXT("No primary") );
			break;
		case D3DFWERR_NOCLIPPER:
			strcpy( strMsg, TEXT("No Clipper") );
			break;
		case D3DFWERR_BADDISPLAYMODE:
			strcpy( strMsg, TEXT("Bad display mode") );
			break;
		case D3DFWERR_NOBACKBUFFER:
			strcpy( strMsg, TEXT("No backbuffer") );
			break;
		case D3DFWERR_NONZEROREFCOUNT:
			strcpy( strMsg, TEXT("Nonzerorefcount") );
			break;
		case D3DFWERR_NORENDERTARGET:
			strcpy( strMsg, TEXT("No render target") );
			break;
		case E_OUTOFMEMORY:
			strcpy( strMsg, TEXT("Not enough memory!") );
			break;
		case DDERR_OUTOFVIDEOMEMORY:
			strcpy( strMsg, TEXT("There was insufficient video memory "
					"to use the\nhardware device.") );
			break;
		default:
			strcpy( strMsg, TEXT("Generic application error.\n\nEnable "
							"debug output for detailed information.") );
	}

	if( MSGERR_APPMUSTEXIT == errType )
	{
		strcat( strMsg, TEXT("\n\nThis program will now exit.") );
		MessageBox( NULL, strMsg, g_strAppTitle, MB_ICONERROR|MB_OK );
	}
	else
	{
		if( MSGWARN_SWITCHTOSOFTWARE == errType )
			strcat( strMsg, TEXT("\n\nSwitching to software rasterizer.") );
		MessageBox( NULL, strMsg, g_strAppTitle, MB_ICONWARNING|MB_OK );
	}
}

