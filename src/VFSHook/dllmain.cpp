#include <Windows.h>

#include "../../lib/ACV/VFS.h"


VOID Start()
{
	HMODULE exe_base = GetModuleHandleW(NULL);
	ACV::VFS::SetResHook((uint32_t)exe_base + 0xCE8E0, (uint32_t)exe_base + 0xD04D0, (uint32_t)exe_base + 0xD0AF0);
	//Attention!! Only one of the three functions can be called at a time
	//SetFileDump("Dump/");
	//SetFileExtract("Extract/");
	//SetFileHook("FileHook/");
	
	/*
	Game:		ハナヒメ＊アブソリュート！
	LoadScript:	rva:0xCE9B0
	ProcScript:	rva:0xB66E0

	Game:		魔法少女まじかるあーりん
	LoadScript:	rva:0xCC1C0
	ProcScript:	rva:0xB3B10
	
	Game:		我が姫君に栄冠を
	LoadScript:	rva:0xD18B0


	Game:		我が姫君に栄冠をFD
	LoadScript:	rva:0xD30F0

	*/

	//SetScriptDump(0xCE9B0, 0xB66E0, "FileHook/Script/");
	//SetScriptHook(0xCE9B0, 0xB66E0, "Dump/Script/");
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