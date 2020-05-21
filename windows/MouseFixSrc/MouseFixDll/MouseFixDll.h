// MouseFixDll

#ifdef MOUSEFIXDLL_EXPORTS
#define MOUSEFIXDLL_API __declspec(dllexport)
#else
#define MOUSEFIXDLL_API __declspec(dllimport)
#endif

// This is exported from the MouseFixDll.dll


typedef void (*logEventStruct)(char *, unsigned int, unsigned int);


extern "C" {
  MOUSEFIXDLL_API bool SetHook(HHOOK hHookNew);
}

extern "C" {
  MOUSEFIXDLL_API LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam);
}

