// MouseFixDll

#define _WIN32_WINNT 0x400
#include <windows.h>
#include <stdio.h>

#include "MouseFixDll.h"



// Shared DATA
#pragma data_seg(".SHARDATA")
static HHOOK hHook = NULL;
static DWORD leftLastTime = 0;
static DWORD middleLastTime = 0;
static DWORD rightLastTime = 0;
#pragma data_seg()


#define LEFT_THRESHOLD 40
#define MIDDLE_THRESHOLD 40
#define RIGHT_THRESHOLD 80


BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}


bool SetHook(HHOOK hHookNew) {
  hHook = hHookNew;
  return true;
}


LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
  bool block = false;
  //if (nCode < 0) ...
  if (nCode == HC_ACTION) {
    PMSLLHOOKSTRUCT pMsllHookStruct = (PMSLLHOOKSTRUCT)lParam;
    DWORD currentTime = pMsllHookStruct->time;
    
	DWORD leftElapsedTime = currentTime - leftLastTime;
	DWORD middleElapsedTime = currentTime - middleLastTime;
	DWORD rightElapsedTime = currentTime - rightLastTime;

	if (wParam == WM_LBUTTONDOWN) {
		if (leftElapsedTime < LEFT_THRESHOLD) {
			//char buffer[1024]; sprintf(buffer, "Left button down @ %d, was %d = %d", (int)currentTime, (int)lastTime, (int)elapsedTime); MessageBox(NULL, buffer, "MouseFixDll", MB_OK);
			block = true;
		}
		else {
			//leftLastTime = currentTime;
		}
	}
	else if (wParam == WM_LBUTTONUP) {
		leftLastTime = currentTime;
	}

	else if (wParam == WM_MBUTTONDOWN) {
		if (middleElapsedTime < MIDDLE_THRESHOLD) {
			//char buffer[1024]; sprintf(buffer, "Left button down @ %d, was %d = %d", (int)currentTime, (int)lastTime, (int)elapsedTime); MessageBox(NULL, buffer, "MouseFixDll", MB_OK);
			block = true;
		}
		else {
			//middleLastTime = currentTime;
		}
	}
	else if (wParam == WM_MBUTTONUP) {
		middleLastTime = currentTime;
	}

	else if (wParam == WM_RBUTTONDOWN) {
		if (rightElapsedTime < RIGHT_THRESHOLD) {
			//char buffer[1024]; sprintf(buffer, "Left button down @ %d, was %d = %d", (int)currentTime, (int)lastTime, (int)elapsedTime); MessageBox(NULL, buffer, "MouseFixDll", MB_OK);
			block = true;
		}
		else {
			//rightLastTime = currentTime;
		}
	}
	else if (wParam == WM_RBUTTONUP) {
		rightLastTime = currentTime;
	}

  }
  if (!block) {
    return CallNextHookEx(hHook, nCode, wParam, lParam);
  } else {
    return 1;
  }
}
