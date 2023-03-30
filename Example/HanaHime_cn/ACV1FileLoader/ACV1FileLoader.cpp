#include <Windows.h>


#include "../ThirdParty/detours/include/detours.h"
#pragma comment(lib,"../ThirdParty/detours/lib.X86/detours.lib")


INT WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR lpCmdLine, _In_ int nShowCmd)
{
	STARTUPINFOW si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	si.cb = sizeof(si);

	LPCSTR dllName = "HanaHime_cn.dll";

	if (DetourCreateProcessWithDllW(L"¥Ï¥Ê¥Ò¥á£ª¥¢¥Ö¥½¥ê¥å©`¥È£¡.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi, dllName, NULL))
	{
		ResumeThread(pi.hThread);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	else
	{
		MessageBoxW(NULL, L"CreateProcess Failed!", NULL, MB_OK);
	}
}