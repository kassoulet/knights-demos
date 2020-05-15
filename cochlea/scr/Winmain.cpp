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
#include "resource.h"

extern "C" unsigned char demo[373760];



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

			StretchBlt(dc,0,ry,w,rh,dcBitmap,0,0,256,128,SRCCOPY);

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


//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//			 message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
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
	{
		char file[1024];
		
		GetTempPath(1000,file);
		strcat(file,"cochlea.exe");

		FILE *f = fopen(file,"wb");
		fwrite(demo,1,sizeof(demo),f);
		fclose(f); 		

		ShellExecute(0,"open",file,0,0,0);

		DeleteFile(file);

		return 0;
	}

//	if (MatchOption(strCmdLine,'c'))
	{
		// We are called to show the configuration window
		ShowConfig(hInst);
		return 0;
	}

return 0;
}






