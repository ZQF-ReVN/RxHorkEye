#include <Windows.h>

#include "../ThirdParty/detours/include/detours.h"
#pragma comment(lib,"../ThirdParty/detours/lib.X86/detours.lib")

STARTUPINFOW si;
PROCESS_INFORMATION pi;

INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int nShowCmd)
{
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	LPCSTR dllName = "ACV1FileHook.dll";

	if (DetourCreateProcessWithDllW(L"¥Ï¥Ê¥Ò¥á£ª¥¢¥Ö¥½¥ê¥å©`¥È£¡.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi, dllName, NULL))
	{
		ResumeThread(pi.hThread);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		MessageBoxW(NULL, L"Drag the game exe file to loader", NULL, MB_OK);
	}

}