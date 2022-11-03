#include "ACV1Extract.h"

VOID Start()
{
	//Attention!! Only one of the three functions can be called at a time
	//SetFileDump();
	//SetFileExtract();
	SetFileHook();
	
	
	/*
	Game:		ハナヒメ＊アブソリュート！
	LoadScript:	rva:0xCE9B0
	DecScript:	rva:0x12AC00
	
	Game:		我が姫君に栄冠を
	LoadScript:	rva:0xD18B0
	DecScript:	rva:0x1389E0

	Game:		我が姫君に栄冠をFD
	LoadScript:	rva:0xD30F0
	DecScript:	rva:0x13A540
	*/
	//SetScriptDump(0xD30F0, 0x13A540);
	//SetScriptHook(0xD30F0, 0x13A540);
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