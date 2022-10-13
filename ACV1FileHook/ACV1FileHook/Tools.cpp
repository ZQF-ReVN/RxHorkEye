#include "Tools.h"

VOID SetConsoleTop()
{
	HWND hConsole = 0;
	WCHAR title[0xFF] = { 0 };

	while (true)
	{
		GetConsoleTitleW(title, 0xFF);
		hConsole = FindWindowW(NULL, (LPWSTR)title);
		if (hConsole != NULL)
		{
			SetWindowPos(hConsole, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			break;
		}
		Sleep(1000);
	}
}

FILE* SetConsole(LPCWSTR lpTitle)
{
	DWORD mode = 0;
	FILE* fpStreamConsole = 0;

	AllocConsole();
	AttachConsole(ATTACH_PARENT_PROCESS);
	freopen_s(&fpStreamConsole, "CONIN$", "r+t", stdin);
	freopen_s(&fpStreamConsole, "CONOUT$", "w+t", stdout);

	SetConsoleTitleW(lpTitle);

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SetConsoleTop, NULL, NULL, NULL);

	GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode);
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode & ~ENABLE_QUICK_EDIT_MODE);

	//system("chcp 65001");
	//setlocale(LC_ALL, "chs");
	std::locale::global(std::locale(""));

	return fpStreamConsole;
}

std::string GetCurrentDirectoryPath()
{
	std::string path;
	CHAR buffer[MAX_PATH] = { 0 };

	GetCurrentDirectoryA(MAX_PATH, buffer);
	path = buffer;
	if (path.empty())
	{
		MessageBoxW(NULL, L"GetCurrentDirectory Failed!!", NULL, NULL);
		ExitProcess(0);
	}
	else
	{
		return path;
	}
}

std::string BackSlash(LPCSTR lpPath)
{
	std::string path = lpPath;

	for (size_t position = 0; position = path.find('/', position); )
	{
		if (position != std::string::npos)
		{
			path[position] = '\\';
		}
		else
		{
			break;
		}
	}

	return path;
}

std::string GetFileName(std::string& strPath)
{
	std::string path;

	size_t position = strPath.find_last_of('\\');
	if (position != std::string::npos)
	{
		return path = strPath.substr(position + 1);
	}
	else
	{
		return strPath;
	}
}

std::string GetFolderPath(std::string& strPath)
{
	std::string path;

	size_t position = strPath.find_last_of('\\');
	if (position != std::string::npos)
	{
		return path = strPath.substr(0, position + 1);
	}
	else
	{
		return "";
	}
}

BOOL WriteMemory(LPVOID lpAddress, LPCVOID lpBuffer, SIZE_T szSize)
{
	DWORD oldProtect = 0;
	BOOL isProtect = FALSE;
	BOOL isWrite = FALSE;

	isProtect = VirtualProtectEx(GetCurrentProcess(), lpAddress, szSize, PAGE_EXECUTE_READWRITE, &oldProtect);
	isWrite = WriteProcessMemory(GetCurrentProcess(), lpAddress, lpBuffer, szSize, NULL);
	if (isProtect && isWrite)
	{
		return TRUE;
	}
	else
	{
		MessageBoxW(NULL, L"WriteMemory Failed!!", NULL, NULL);
		return FALSE;
	}
}

BOOL SetHook(DWORD dwRawAddr, DWORD dwTarAddr, SIZE_T szRawSize)
{
	BOOL isProtect = 0;
	DWORD oldProtect = 0;
	PBYTE allocateAddr = 0;
	DWORD relativeAddr = 0;
	BYTE rawJmp[] = { 0xE9,0x00,0x00,0x00,0x00 };
	BYTE retJmp[] = { 0xE9,0x00,0x00,0x00,0x00 };
	BYTE tarCal[] = { 0xE8,0x00,0x00,0x00,0x00 };

	isProtect = VirtualProtect((LPVOID)dwRawAddr, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect);
	allocateAddr = (PBYTE)VirtualAlloc(NULL, 0x1000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (allocateAddr && isProtect)
	{
		//Copy the Code for the original address to alloc address
		memcpy(allocateAddr, (PVOID)dwRawAddr, szRawSize);

		//Write Jmp Code
		relativeAddr = (DWORD)allocateAddr - dwRawAddr - sizeof(rawJmp);
		memcpy(&rawJmp[1], &relativeAddr, sizeof(DWORD));
		memcpy((PBYTE)dwRawAddr, rawJmp, sizeof(rawJmp));

		//Write Call TarFunc Code
		relativeAddr = dwTarAddr - (DWORD)(&allocateAddr[szRawSize]) - sizeof(tarCal);
		memcpy(&tarCal[1], &relativeAddr, sizeof(DWORD));
		memcpy(&allocateAddr[szRawSize], tarCal, sizeof(tarCal));

		//Write Ret Code
		relativeAddr = (dwRawAddr + szRawSize) - (DWORD)(&allocateAddr[szRawSize + sizeof(tarCal)]) - sizeof(retJmp);
		memcpy(&retJmp[1], &relativeAddr, sizeof(DWORD));
		memcpy(&allocateAddr[szRawSize + sizeof(tarCal)], retJmp, sizeof(retJmp));

		return TRUE;
	}
	else
	{
		MessageBoxW(NULL, L"SetHook Failed!!", NULL, NULL);
		return FALSE;
	}

}

BOOL DetourAttachFunc(PVOID ppPointer, PVOID pDetour)
{
	LONG erroAttach = 0;
	LONG erroCommit = 0;

    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

	erroAttach = DetourAttach((PVOID*)ppPointer, pDetour);
	erroCommit = DetourTransactionCommit();

	if (erroAttach != NO_ERROR || erroCommit != NO_ERROR)
	{
		MessageBoxW(NULL, L"DetourAttach Failed!!", NULL, NULL);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL DetourDetachFunc(PVOID ppPointer, PVOID pDetour)
{
	LONG erroDetach = 0;
	LONG erroCommit = 0;

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	erroDetach = DetourDetach((PVOID*)ppPointer, pDetour);
	erroDetach = DetourTransactionCommit();
	if (erroDetach != NO_ERROR || erroCommit != NO_ERROR)
	{
		MessageBoxW(NULL, L"DetourDetach Failed!!", NULL, NULL);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
