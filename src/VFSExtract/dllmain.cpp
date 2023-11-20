#include <Windows.h>

#include "../../lib/ACV/VFS.h"


VOID Start()
{
	HMODULE exe_base = GetModuleHandleW(NULL);
	ACV::VFS::SetExtract((uint32_t)exe_base + 0xD8640, (uint32_t)exe_base + 0xCC020);
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