#include "ACV1Extract.h"

VOID Start()
{
	//Attention!! Only one of these functions can be called at a time
	//SetFileDump();
	//SetFileExtract();
	//SetFileHook();
	//SetScriptDump();
	SetScriptHook();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Start();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

VOID __declspec(dllexport) DirA() {}