// Bow-Man_Engine3.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Bow-Man_Engine3.h"
#include "Framework.h"
#include "D2DFramework.h"
#include "ChatDataBase.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE	hInst;                                // current instance
WCHAR		szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR		szWindowClass[MAX_LOADSTRING];            // the main window class name

//Main Framework
CFramework	gFramework;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BOWMAN_ENGINE3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BOWMAN_ENGINE3));


    MSG msg;
//	Main message loop:
//	while (GetMessage(&msg, nullptr, 0, 0))
	while (gbLoop){
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else{
			gFramework.FrameAdvance();
		}
	}


    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BOWMAN_ENGINE3));
    wcex.hCursor        = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR2));
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BOWMAN_ENGINE3);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// Store instance handle in our global variable
	hInst = hInstance; 

	FRAME_BUFFER_WIDTH = GetSystemMetrics(SM_CXSCREEN);
	FRAME_BUFFER_HEIGHT = GetSystemMetrics(SM_CYSCREEN);
//	FRAME_BUFFER_WIDTH = 1000;
//	FRAME_BUFFER_HEIGHT = 750;
	FRAME_BUFFER_POS_X = 0;
	FRAME_BUFFER_POS_Y = 0;

	gWindowRect = { FRAME_BUFFER_POS_X, FRAME_BUFFER_POS_Y, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
//	DWORD dwStyle = WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_THICKFRAME;
	DWORD dwStyle = WS_POPUP;
	bool bMenue = false;
//	AdjustWindowRect(&gWindowRect, dwStyle, bMenue); 

	gHwnd = CreateWindow(szWindowClass, szTitle, dwStyle, gWindowRect.left, gWindowRect.top, gWindowRect.right, gWindowRect.bottom, nullptr, nullptr, hInstance, nullptr);
	if(!bMenue) SetMenu(gHwnd, NULL);

//	SetWindowPos(gHwnd, HWND_TOPMOST, 0,  0, GetSystemMetrics(SM_CXSCREEN ), GetSystemMetrics( SM_CYSCREEN ), 0L );

	POINT pos{FRAME_BUFFER_POS_X, FRAME_BUFFER_POS_Y};
	ScreenToClient(gHwnd, &pos);
	FRAME_BUFFER_POS_X = FRAME_BUFFER_POS_X - pos.x;
	FRAME_BUFFER_POS_Y = FRAME_BUFFER_POS_Y - pos.y;

	GetClientRect(gHwnd, &gWindowRect);
	FRAME_BUFFER_WIDTH = gWindowRect.right;
	FRAME_BUFFER_HEIGHT = gWindowRect.bottom;

	if (!gHwnd) return FALSE;
   
	//wcout 한글 출력
	std::wcout.imbue(std::locale("kor")); 

	//Enable Drag Drop in Window
	DragAcceptFiles(gHwnd, TRUE);
	srand(time(NULL));

	ShowWindow(gHwnd, SW_MAXIMIZE);
//	ShowWindow(gHwnd, nCmdShow);
//	UpdateWindow(gHwnd);

	gFramework.Initialize(gHwnd);
	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_DROPFILES:
		gFramework.DragDropProc(hWnd, message, wParam, lParam);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_SIZE: {
		POINT pos{ FRAME_BUFFER_POS_X, FRAME_BUFFER_POS_Y };
		ScreenToClient(gHwnd, &pos);
		FRAME_BUFFER_POS_X = FRAME_BUFFER_POS_X - pos.x;
		FRAME_BUFFER_POS_Y = FRAME_BUFFER_POS_Y - pos.y;

		GetClientRect(gHwnd, &gWindowRect);
		FRAME_BUFFER_WIDTH = gWindowRect.right;
		FRAME_BUFFER_HEIGHT = gWindowRect.bottom;
		break;
	}
	case WM_EXITSIZEMOVE: {
		POINT pos{ FRAME_BUFFER_POS_X, FRAME_BUFFER_POS_Y };
		ScreenToClient(gHwnd, &pos);
		FRAME_BUFFER_POS_X = FRAME_BUFFER_POS_X - pos.x;
		FRAME_BUFFER_POS_Y = FRAME_BUFFER_POS_Y - pos.y;

		GetClientRect(gHwnd, &gWindowRect);
		FRAME_BUFFER_WIDTH = gWindowRect.right;
		FRAME_BUFFER_HEIGHT = gWindowRect.bottom;
		break;
	}
	case WM_MOUSEHOVER: 
	//	SetCapture(hWnd);
	//	SetCursor(NULL);
		break;
	case WM_MOUSELEAVE:
		ReleaseCapture();
		SetCursor(LoadCursor(0, MAKEINTRESOURCE(IDC_CURSOR2)));
		break;
	case WM_MOUSEMOVE:
		TRACKMOUSEEVENT hoverEvent;
		hoverEvent.cbSize = sizeof(hoverEvent);
		hoverEvent.dwFlags = TME_HOVER;
		hoverEvent.hwndTrack = hWnd;
		hoverEvent.dwHoverTime = 1000;
		TrackMouseEvent(&hoverEvent);

		TRACKMOUSEEVENT leaveEvent;
		leaveEvent.cbSize = sizeof(leaveEvent);
		leaveEvent.dwFlags = TME_LEAVE;
		leaveEvent.hwndTrack = hWnd;
		TrackMouseEvent(&leaveEvent);
	case WM_LBUTTONDOWN:	case WM_LBUTTONUP:	case WM_RBUTTONDOWN:	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:	case WM_MBUTTONUP:	case WM_RBUTTONDBLCLK: case WM_LBUTTONDBLCLK: case WM_MBUTTONDBLCLK:
	case WM_MOUSEWHEEL:		// case WM_MYMSG:
		gFramework.MouseInputProc(hWnd, message, wParam, lParam);
		break;
	case WM_CHAR:			case WM_IME_CHAR:
	case WM_IME_SETCONTEXT:	case WM_IME_STARTCOMPOSITION:	case WM_IME_ENDCOMPOSITION:
	case WM_IME_CONTROL:	case WM_IME_COMPOSITION:		case WM_IME_NOTIFY:
	case WM_KEYDOWN: case WM_KEYUP:
		gFramework.KeyboardInputProc(hWnd, message, wParam, lParam);
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
		gbLoop = false;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
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
