// ProjectOxford.cpp : Defines the entry point for the application.
// This program renders a small set of adjacent hypercubes using 4D to 3D perspective projection
// and 3D to 2D perspective projection
// The 4D object can be rotated, and the 3D camera can be rotated and translated
// The 3D projection is depthcued, so that point which are further away appear dimmer
// The 3D projection can be rendered stereoscopically, for use with wall-eyed viewing

#include "stdafx.h"
#include <cmath>
#include <string>
#include <iostream>
#include <glload/gl_3_3.h>
#include <glload/gl_load.h>
#include <glload/wgl_all.h>
#include <glload/wgl_load.h>

#include "ProjectOxford.h"
#include "drawing.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#define MAX_LOADSTRING 100

// Global Variables:
struct Globals{
	//Instance handle
	HINSTANCE hInstance;

	//Strings for window classes
	TCHAR szTitle[MAX_LOADSTRING];
	TCHAR szWindowClass[MAX_LOADSTRING];
	TCHAR szWindowClass_dummy[MAX_LOADSTRING];

	//Window handles
	HWND hWnd;
	HWND hWnd_dummy;

	//Device context handles
	HDC hdc;
	HDC hdc_dummy;

	//OpenGL rendering context handles
	HGLRC hglrc1;
	HGLRC hglrc_dummy;

	//Desired width and height of the window CLIENT AREA
	int width, height;
};

Globals g;

// Forward declarations of functions included in this code module:
ATOM				registerDummyClass(HINSTANCE);
ATOM				registerMainClass(HINSTANCE hInstance);
BOOL				setupDummyWindow(HINSTANCE, int);
BOOL				setupMainWindow(HINSTANCE, int);
BOOL				setupDummyPixelFormat(HINSTANCE);
BOOL				setupMainPixelFormat(HINSTANCE, int);
int					getRealPixelFormat(HINSTANCE);
void				render();
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	WndProc_Dummy(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow){

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	HACCEL hAccelTable;
	// Initialize global strings
	LoadString(hInstance, IDS_CLASSNAME_MAIN, g.szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_CLASSNAME_DUMM, g.szWindowClass_dummy, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_TITLE, g.szTitle, MAX_LOADSTRING);

	registerDummyClass(hInstance);
	registerMainClass(hInstance);

	g.hInstance = hInstance;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECTOXFORD));

	//Make dummy context for loading gl extensions
	if (!setupDummyWindow(hInstance, nCmdShow)){
		return FALSE;
	}
	g.hdc_dummy = GetDC(g.hWnd_dummy);
	if (!setupDummyPixelFormat(hInstance)){
		return FALSE;
	}
	g.hglrc_dummy = wglCreateContext(g.hdc_dummy);
	wglMakeCurrent(g.hdc_dummy, g.hglrc_dummy);

	//Load the opengl functions and extensions
	ogl_LoadFunctions();
	wgl_LoadFunctions(g.hdc_dummy);

	//Destroy dummy context
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(g.hglrc_dummy);
	ReleaseDC(g.hWnd_dummy, g.hdc_dummy);
	DestroyWindow(g.hWnd_dummy);

	//Setup the real program window
	if (!setupMainWindow(hInstance, nCmdShow)){
		return FALSE;
	}
	g.hdc = GetDC(g.hWnd);
	//Use the loaded gl extensions to get a pixel format with multisampling
	int pixelFormat = getRealPixelFormat(hInstance);
	//Set the pixel format to the main window
	if (!setupMainPixelFormat(hInstance, pixelFormat)){
		return FALSE;
	}
	g.hglrc1 = wglCreateContext(g.hdc);
	wglMakeCurrent(g.hdc, g.hglrc1);

	init(g.width, g.height);
	reshape(g.width, g.height);

	MSG msg;
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			//Draw using OpenGL
			update(.01);
			wglMakeCurrent(g.hdc, g.hglrc1);
			
			//There is an option in render method for stereoscopic rendering
			render(g.width, g.height, 1);
			render(g.width, g.height, 2);
			SwapBuffers(g.hdc);
		}
	}

	return (int) msg.wParam;
}

ATOM registerDummyClass(HINSTANCE hInstance){
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc_Dummy;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECTOXFORD));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_PROJECTOXFORD);
	wcex.lpszClassName = g.szWindowClass_dummy;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

ATOM registerMainClass(HINSTANCE hInstance){
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECTOXFORD));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PROJECTOXFORD);
	wcex.lpszClassName	= g.szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//Create a dummy window
BOOL setupDummyWindow(HINSTANCE hInstance, int nCmdShow)
{
	RECT rect;
	SetRect(&rect, 150,  // left
		50,  // top
		1450, // right
		650); // bottom

	g.width = rect.right - rect.left;
	g.height = rect.bottom - rect.top;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	g.hWnd_dummy = CreateWindow(g.szWindowClass_dummy, g.szTitle, WS_OVERLAPPEDWINDOW,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);

	if (!g.hWnd_dummy)
	{
		return FALSE;
	}

	return TRUE;
}

//Create the main window of the program
BOOL setupMainWindow(HINSTANCE hInstance, int nCmdShow)
{   
	RECT rect;
	SetRect(&rect, 150,  // left
	   50,  // top
	   1450, // right
	   650); // bottom

	g.width = rect.right - rect.left;
	g.height = rect.bottom - rect.top;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	g.hWnd = CreateWindow(g.szWindowClass, g.szTitle, WS_OVERLAPPEDWINDOW,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);

	if (!g.hWnd){
		return FALSE;
	}

	ShowWindow(g.hWnd, nCmdShow);
	UpdateWindow(g.hWnd);

	return TRUE;
}

//Sets the pixel format of the dummy window
BOOL setupDummyPixelFormat(HINSTANCE hInstance){
	PIXELFORMATDESCRIPTOR pfd = { 0 };

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;

	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 24;
	
	int chosenPixelFormat = ChoosePixelFormat(g.hdc_dummy, &pfd);

	if (chosenPixelFormat == 0){
		return FALSE;
	}

	int result = SetPixelFormat(g.hdc_dummy, chosenPixelFormat, &pfd);

	if (result == NULL){
		return FALSE;
	}

	return TRUE;
}

//Sets the pixel format of the main window's device context
BOOL setupMainPixelFormat(HINSTANCE hInstance, int chosenPixelFormat){
	PIXELFORMATDESCRIPTOR pfd = { 0 };

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW;

	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32; // This count of color bits EXCLUDES alpha.
	pfd.cDepthBits = 24;

	if (chosenPixelFormat == 0){
		return FALSE;
	}

	int result = SetPixelFormat(g.hdc, chosenPixelFormat, &pfd);

	if (result == NULL){
		return FALSE;
	}

	return TRUE;
}

//Gets a pixel format with multisampling, to be used in the main window's device context
int getRealPixelFormat(HINSTANCE hInstance){
	const int attribList[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, 1, //Number of buffers
		WGL_SAMPLES_ARB, 8,        //Number of samples
		0
	};

	int pixelFormat;
	UINT numFormats = 0;

	wglChoosePixelFormatARB(g.hdc, attribList, NULL, 1, &pixelFormat, &numFormats);

	return pixelFormat;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(g.hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CHAR:
		processKeyDown(wParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_SIZE:
		g.height = HIWORD(lParam);
		g.width = LOWORD(lParam);
		reshape(g.width, g.height);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//WndProc for Dummy Window
//Avoids calling PostQuitMessage() when destroying the dummy window
LRESULT CALLBACK WndProc_Dummy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message){
	case WM_DESTROY:
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}