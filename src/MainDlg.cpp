
#include "stdafx.h"
#include "MainDlg.h"

void SetIcon(HINSTANCE hInst, HWND hWnd, int icon_id){
  HANDLE hIcon;

  hIcon = LoadImage( hInst,
                   MAKEINTRESOURCE(icon_id),
                   IMAGE_ICON,
                   GetSystemMetrics(SM_CXSMICON),
                   GetSystemMetrics(SM_CYSMICON),
                   0);
  if(hIcon){
     SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
  }
}
