#include <Windows.h>

#include "../../lib/HorkEye/VFS.h"


static void Start()
{
	HMODULE exe_base = GetModuleHandleW(NULL);

	// 宿星のガールフレンド 芙慈子編 PKG 1.0
	// HorkEye::VFS::SetHook((uint32_t)exe_base + 0xCE8E0, (uint32_t)exe_base + 0xD04D0, (uint32_t)exe_base + 0xD0AF0);

	// 魔法少女まじかるあーりん PKG 1.0
	HorkEye::VFS::SetHook((uint32_t)exe_base + 0xBC340, (uint32_t)exe_base + 0xCC1C0, (uint32_t)exe_base + 0xB3B10);
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

void __declspec(dllexport) DirA() {}