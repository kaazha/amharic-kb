// AmharicKeyboard.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "AmharicKeyboard.h"
#include "keymapper.h"
#include "maindlg.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HWND hWnd;
BYTE keyMapperEnabled = 1;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
int					ProcessRawInput(LPARAM lParam);
BOOL CALLBACK DialogProc (HWND hwnd, UINT message,  WPARAM wParam,  LPARAM lParam);

/*
typedef void (*callback)();
typedef void (*Install)();
typedef void (*Uninstall)();
Uninstall uninstall;
HHOOK hhk;*/

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	 
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_AMHARICKEYBOARD, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
		
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AMHARICKEYBOARD));
	
	/*
	 // install keyboard hook
	 HINSTANCE hDll = LoadLibrary(_T("KeyboardHookProcDll.dll")); 
	 if (hDll == NULL) 
	 {
		//printf("unable to load hook callback dll.");
		 MessageBox(0, _T("unable to load hook callback dll."), _T("Error"), 0);
	 } 
	 else {
		Install install = (Install) GetProcAddress(hDll, "install");
		uninstall = (Uninstall) GetProcAddress(hDll, "uninstall");
		install();
		//HOOKPROC hp = (HOOKPROC)GetProcAddress(hDll, "HookProc");
		//hhk = SetWindowsHookEx(WH_GETMESSAGE, hp, hDll, NULL);
	 }
	*/

	RAWINPUTDEVICE rawInputDevices[1];
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; // Keyboard
	rid.usUsage = 0x06;  // Keyboard
	rid.dwFlags = RIDEV_INPUTSINK;   // adds HID keyboard and also ignores legacy keyboard messages
	rid.hwndTarget = hWnd;
	rawInputDevices[0] = rid;

	if (RegisterRawInputDevices(rawInputDevices, 1, sizeof(rid)) == FALSE) {
		//registration failed. Call GetLastError for the cause of the error
		MessageBox(0, _T("RegisterRawInputDevices failed."), _T("Error"), 0);
	}
	else {
		InitKeyMapper();
	}
	
	int status;
    while ((status = GetMessage(&msg, 0, 0, 0)) != 0)
    {
        if (status == -1)
            return -1;
        if (!IsDialogMessage (hWnd, &msg))
        {
            TranslateMessage ( &msg );
            DispatchMessage ( &msg );
        }
    }

    return msg.wParam;

	/*
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	// uninstall();
	//UnhookWindowsHookEx(hhk);

	return (int) msg.wParam;*/
}

BOOL CALLBACK DialogProc (HWND hwnd, 
                          UINT message, 
                          WPARAM wParam, 
                          LPARAM lParam)
{
    //static Controller* control = 0;
    switch (message)
    {
	case WM_INPUT:
		//if(keyMapperEnabled == TRUE) {
			ProcessRawInput(lParam);
		//}
		break;
    case WM_INITDIALOG:
		SetIcon(hInst, hwnd, IDI_AMHARICKEYBOARD);
        return TRUE;
		break;
    case WM_COMMAND:
		switch(LOWORD(wParam))
		{
			case ID_ABOUT1:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case ID_EXIT:
				EndDialog(hwnd, LOWORD(wParam));
				PostQuitMessage(0);
				break;
			case ID_ENABLE:
				keyMapperEnabled = TRUE; 
				break;
			case ID_HELP:
				// shell execute help.html
				ShellExecute(NULL, _T("open"), _T("help.html"), NULL, NULL, SW_SHOWNORMAL); 
				break;
			case ID_DISABLE:
				keyMapperEnabled = FALSE; 
				break;
		}
		return TRUE;
		break;
    case WM_HSCROLL:
        //control->Scroll (hwnd, LOWORD (wParam), HIWORD (wParam));
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;
    case WM_CLOSE:
        //delete control;
        DestroyWindow (hwnd);
        return TRUE;
    }
    return FALSE;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AMHARICKEYBOARD));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_AMHARICKEYBOARD);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
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
   hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MAINDLG), 0,  DialogProc);
	
	/*
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	*/

   if (!hWnd)
   {
      return FALSE;
   }
	
   //ShowWindow(hWnd, nCmdShow);
  // UpdateWindow(hWnd);

   return TRUE;
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_INPUT:
		if(keyMapperEnabled == TRUE) {
			ProcessRawInput(lParam);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
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
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


int ProcessRawInput(LPARAM lParam){
    UINT dwSize;

    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize,  sizeof(RAWINPUTHEADER));
    LPBYTE lpb = new BYTE[dwSize];
    if (lpb == NULL) 
    {
        return 0;
    } 

	UINT sz = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

    if (sz == dwSize)
	{
		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEKEYBOARD) 
		{
			int ctrl = GetAsyncKeyState(VK_CONTROL);
			UINT msg = raw->data.keyboard.Message;
			int vkey = raw->data.keyboard.VKey;
			if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)  {
				if (ctrl && (vkey == VK_MENU)){
					keyMapperEnabled = 1 - keyMapperEnabled;		// CTRL+ALT: Toggle Enable/Disable Amharic 
				}
				else if (keyMapperEnabled && !ctrl) {
					ProcessKey(vkey);
				}
			}
		}
	}

    delete[] lpb; 
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
