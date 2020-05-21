// MouseFix

#define _WIN32_WINNT 0x400
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

#include "../MouseFixDll/MouseFixDll.h"



#define CLASSNAME "MouseFixClass"
#define APPLICATIONNAME "MouseFix"



//// STANDARD DEBUGGING STUFF ////
#include <stdio.h>
#include <stdarg.h>
// dprintf -- Prints formatted debug string to the console (if one has been created)
static int dprintf( char *format, ... )
{
	char string[1025];
	va_list va; va_start( va, format ); _vsnprintf( string, 1024, format, va ); va_end( va );
	string[1024] = 0;	// Got a feeling that *nprintf() variants don't include last NULL if they reach maximum size (may be wrong, doesn't hurt anyway).
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 
  if (!hStdout) { AllocConsole(); hStdout = GetStdHandle(STD_OUTPUT_HANDLE); }
	DWORD cWritten = 0;
	return !WriteFile( hStdout, string, strlen(string), &cWritten, NULL );
}
/////////////////////////////////



// Global data
HWND ghApp = NULL;
HINSTANCE hMod = NULL;
HOOKPROC lpfn = NULL;
HHOOK hHook = NULL;

typedef bool (*SetHookType)(HHOOK);
SetHookType setHook = NULL;


void Msg(TCHAR *szFormat, ...) {
  TCHAR szBuffer[1024];  // Large buffer for long filenames or URLs
  const size_t NUMCHARS = sizeof(szBuffer) / sizeof(szBuffer[0]);
  const int LASTCHAR = NUMCHARS - 1;

  // Format the input string
  va_list pArgs;
  va_start(pArgs, szFormat);

  // Use a bounded buffer size to prevent buffer overruns.  Limit count to
  // character size minus one to allow for a NULL terminating character.
  _vsnprintf(szBuffer, NUMCHARS - 1, szFormat, pArgs);
  va_end(pArgs);

  // Ensure that the formatted string is NULL-terminated
  szBuffer[LASTCHAR] = TEXT('\0');

  MessageBox(NULL, szBuffer, TEXT("Message"), MB_OK | MB_ICONERROR);
}


void Create(HWND hWnd) {

  hMod = LoadLibrary("MouseFixDll.dll");
  //if (hMod == NULL) { hMod = LoadLibrary("..\\..\\MouseFixDll\\Debug\\MouseFixDll.dll"); }
  if (hMod == NULL) {
    Msg("Library (MouseFixDll.dll) not found");
    PostMessage(hWnd, WM_CLOSE, 0, 0);
  } else {
    lpfn = (HOOKPROC)GetProcAddress(hMod, "_LowLevelMouseProc@12");
    setHook = (SetHookType)GetProcAddress(hMod, "SetHook");
    if (lpfn == NULL || setHook == NULL) {
      Msg("Function(s) not found");
    } else {
      hHook = SetWindowsHookEx(WH_MOUSE_LL, lpfn, hMod, 0);
      setHook(hHook);
    }
  }

}


void Destroy() {

  if (hHook) {
    UnhookWindowsHookEx(hHook);
    hHook = NULL;
    if (setHook != NULL) {
      setHook(NULL);
    }
  }

  if (hMod) {
    FreeLibrary(hMod);
    hMod = NULL;
    lpfn = NULL;
  }
}


LRESULT CALLBACK WndMainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
    case WM_CREATE:
      Create(hWnd);
      break;

    case WM_KEYDOWN:
      if (wParam == VK_ESCAPE) {
        PostMessage(hWnd, WM_CLOSE, 0, 0);
      }
      break;

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstP, LPSTR lpCmdLine, int nCmdShow) {
  MSG msg = {0};
  WNDCLASS wc;

  // Register the window class
  ZeroMemory(&wc, sizeof wc);
  wc.lpfnWndProc   = WndMainProc;
  wc.hInstance     = hInstance;
  wc.lpszClassName = CLASSNAME;
  wc.lpszMenuName  = NULL;
  wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

  if(!RegisterClass(&wc)) {
    Msg(TEXT("RegisterClass Failed! Error=0x%x\r\n"), GetLastError());
    exit(1);
  }

  // Create the main window.  The WS_CLIPCHILDREN style is required.
  ghApp = CreateWindow(CLASSNAME, APPLICATIONNAME,
                       WS_CAPTION | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       0, 0, hInstance, 0);

  if(ghApp) {
    // Create window

nCmdShow = SW_HIDE;

    ShowWindow(ghApp, nCmdShow);

    // Main message loop
    while(GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  Destroy();

  return (int)msg.wParam;
}



