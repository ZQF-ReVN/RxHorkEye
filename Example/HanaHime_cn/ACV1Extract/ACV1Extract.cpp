#include "ACV1Extract.h"
#include "../TDA/ConsoleX.h"
#include "../TDA/DetoursX.h"
#include "../TDA/FileX.h"
#include "../TDA/MemX.h"


static FILE* g_fpFileNameList = NULL;
static DWORD g_dwExeBase = (DWORD)GetModuleHandleW(NULL);


//Game Engine Export This Func
typedef DWORD(CDECL* pLoadFile)(LPCSTR lpFileName, PBYTE* ppFileBuffer, PDWORD dwFileSize, PDWORD dwUnknow);
pLoadFile RawLoadFile = (pLoadFile)GetProcAddress(GetModuleHandleW(NULL), "loadFile");

typedef DWORD(CDECL* pLoadFile2)(LPCSTR lpFileName, LPVOID* pInfo);
pLoadFile2 RawLoadFile2 = NULL;

//To Find This Func Search String "Script.dat"
typedef DWORD(CDECL* pLoadScript)(DWORD dwHashHigh, DWORD dwHashLow, PBYTE* ppBuffer);
pLoadScript RawLoadScript = NULL;

//To Find This Func Search String "script/"
typedef DWORD(CDECL* pProcScript)(PBYTE pScriptBuffer, DWORD dwScriptSize, PDWORD pFunc, PDWORD pRetStruct, LPCSTR lpString);
pProcScript RawProcScript = NULL;

BOOL ACV1FileWrite(LPCSTR lpRelativePath, PBYTE* ppFileBuffer, PDWORD dwFileSize)
{
	static const std::string current = TDA::FileX::GetCurrentDirectoryFolder_RETA();
	std::string filePath = current + TDA::FileX::BackSlash_RET(lpRelativePath);
	std::string fileFolder = TDA::FileX::PathRemoveFileName_RET(filePath);

	SHCreateDirectoryExA(NULL, fileFolder.c_str(), NULL);

	HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return FALSE;

	WriteFile(hFile, *ppFileBuffer, *dwFileSize, NULL, NULL);
	FlushFileBuffers(hFile);
	CloseHandle(hFile);

	return TRUE;
}

static LPCSTR g_lpFileDumpFolder = NULL; static CHAR g_lpFileDumpPath[MAX_PATH] = { 0 };
DWORD ACV1FileDump(LPCSTR lpRelativePath, PBYTE* ppFileBuffer, PDWORD dwFileSize, PDWORD dwUnknow)
{
	DWORD result = RawLoadFile(lpRelativePath, ppFileBuffer, dwFileSize, dwUnknow);
	if ((ppFileBuffer[0] == NULL) || (dwFileSize[0] == NULL)) return result;

	lstrcpyA(g_lpFileDumpPath, g_lpFileDumpFolder); lstrcatA(g_lpFileDumpPath, lpRelativePath);
	ACV1FileWrite(g_lpFileDumpPath, ppFileBuffer, dwFileSize);

	TDA::ConsoleX::PutConsoleA("%s\n", g_lpFileDumpPath);

	fprintf(g_fpFileNameList, "%s\n", lpRelativePath);
	fflush(g_fpFileNameList);

	return result;
}

static std::string g_strFileExtractFolder;
VOID ACV1FileExtract()
{
	PBYTE buffer = 0;
	DWORD size = 0;
	DWORD unknow = 0;
	static std::string fileName;

	while (true)
	{
		std::cout << "FileName:";
		std::cin >> fileName;

		RawLoadFile(fileName.c_str(), &buffer, &size, &unknow);
		if (ACV1FileWrite((g_strFileExtractFolder + fileName).c_str(), &buffer, &size))
		{
			TDA::ConsoleX::PutConsoleA("Extract:%s\n", fileName);
		}
		else
		{
			TDA::ConsoleX::PutConsoleA("%s\n", "Extract Failed!!");
		}

		size = 0;
		unknow = 0;
	}
}

//for this engine if can't find the hash value of filename in pack, will read the file in game directory.
//An error will be reported if the file does not exist in the directory either.
//So I changed the file name path so that the engine could not search for the corresponding file in pack
//and thus go to the directory to read the file
static LPCSTR g_lpFileHookFolder = NULL; static CHAR g_lpFileHookPath[MAX_PATH] = { 0 };
BOOL FileExist(LPCSTR lpRelativePath)
{
	lstrcpyA(g_lpFileHookPath, g_lpFileHookFolder); lstrcatA(g_lpFileHookPath, lpRelativePath);

	DWORD attri = GetFileAttributesA(g_lpFileHookPath);
	if ((attri != INVALID_FILE_ATTRIBUTES) && (attri != FILE_ATTRIBUTE_DIRECTORY))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

DWORD ACV1FileHook(LPCSTR lpRelativePath, PBYTE* ppFileBuffer, PDWORD dwFileSize, PDWORD dwUnknow)
{
	if (FileExist(lpRelativePath)) lpRelativePath = g_lpFileHookPath;
	return RawLoadFile(lpRelativePath, ppFileBuffer, dwFileSize, dwUnknow);
}

DWORD ACV1FileHook2(LPCSTR lpRelativePath, LPVOID* pInfo)
{
	if (FileExist(lpRelativePath)) lpRelativePath = g_lpFileHookPath;
	return RawLoadFile2(lpRelativePath, pInfo);
}

static CHAR g_lpHashName[0x11] = { 0 };
DWORD ACV1ScriptLoad(DWORD dwHashHigh, DWORD dwHashLow, PBYTE* ppBuffer)
{
	sprintf_s(g_lpHashName, 0x11, "%X%X", dwHashHigh, dwHashLow);
	return RawLoadScript(dwHashHigh, dwHashLow, ppBuffer);
}

static LPCSTR g_lpScriptDumpFolder; static CHAR g_lpScriptDumpPath[MAX_PATH] = { 0 };
DWORD ACV1ScriptDump(PBYTE pScriptBuffer, DWORD dwScriptSize, PDWORD pFunc, PDWORD pRetStruct, LPCSTR lpString)
{
	if (g_lpHashName[0])
	{
		lstrcpyA(g_lpScriptDumpPath, g_lpScriptDumpFolder); lstrcatA(g_lpScriptDumpPath, g_lpHashName); memset(g_lpHashName, 0x0, 0x11);
		HANDLE hFile = CreateFileA(g_lpScriptDumpPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			TDA::ConsoleX::PutConsoleA("%s\n", g_lpScriptDumpPath);
			WriteFile(hFile, pScriptBuffer, dwScriptSize, NULL, NULL);
			FlushFileBuffers(hFile);

			CloseHandle(hFile);
		}
	}

	return RawProcScript(pScriptBuffer, dwScriptSize, pFunc, pRetStruct, lpString);
}

static DWORD g_dwAllocSize = 0; static PBYTE g_pAllocBuffer = NULL;
static LPCSTR g_lpScriptHookFolder; static CHAR g_lpScriptHookPath[MAX_PATH] = { 0 };
DWORD ACV1ScriptHook(PBYTE pScriptBuffer, DWORD dwScriptSize, PDWORD pFunc, PDWORD pRetStruct, LPCSTR lpString)
{
	if (g_lpHashName[0])
	{
		lstrcpyA(g_lpScriptHookPath, g_lpScriptHookFolder); lstrcatA(g_lpScriptHookPath, g_lpHashName); memset(g_lpHashName, 0x0, 0x11);
		HANDLE hFile = CreateFileA(g_lpScriptHookPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD szNewScript = GetFileSize(hFile, NULL);
			if (g_dwAllocSize < szNewScript)
			{
				if (g_pAllocBuffer) VirtualFree(g_pAllocBuffer, NULL, MEM_RELEASE);
				g_pAllocBuffer = (PBYTE)VirtualAlloc(NULL, szNewScript, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				g_dwAllocSize = szNewScript;
			}

			DWORD read = 0;
			if (ReadFile(hFile, g_pAllocBuffer, szNewScript, &read, NULL) && szNewScript)
			{
				pScriptBuffer = g_pAllocBuffer;
				dwScriptSize = szNewScript;
			}

			CloseHandle(hFile);
		}
	}

	return RawProcScript(pScriptBuffer, dwScriptSize, pFunc, pRetStruct, lpString);
}


//Extract files from a running game
VOID SetFileDump(LPCSTR lpFolder)
{
	TDA::ConsoleX::SetConsole(L"ACV1FileDump");

	g_lpFileDumpFolder = lpFolder;

	g_fpFileNameList = _fsopen("ACV1FileNameList.txt", "a", _SH_DENYNO);
	if (!g_fpFileNameList) MessageBoxW(NULL, L"CreateListFile Failed!!", NULL, NULL);

	TDA::DetoursX::DetourAttachFunc(&RawLoadFile, ACV1FileDump);
}

//Extract files by filename
VOID SetFileExtract(LPCSTR lpFolder)
{
	TDA::ConsoleX::SetConsole(L"ACV1FileExtract");

	g_strFileExtractFolder = lpFolder;

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ACV1FileExtract, NULL, NULL, NULL);
}

//Read files without repack
VOID SetFileHook(LPCSTR lpFolder, DWORD rvaLoadFile2)
{
	//TDA::ConsoleX::SetConsole(L"ACV1FileHook");

	CreateDirectoryA(lpFolder, NULL);

	g_lpFileHookFolder = lpFolder;
	RawLoadFile2 = (pLoadFile2)(g_dwExeBase + rvaLoadFile2);
	TDA::DetoursX::DetourAttachFunc(&RawLoadFile, ACV1FileHook);
	TDA::DetoursX::DetourAttachFunc(&RawLoadFile2, ACV1FileHook2);
}

//Extract script at startup
VOID SetScriptDump(DWORD rvaLoadScript, DWORD rvaProcScript, LPCSTR lpFolder)
{
	TDA::ConsoleX::SetConsole(L"ACV1ScriptDump");

	g_lpScriptDumpFolder = lpFolder;

	SHCreateDirectoryExA(NULL, (TDA::FileX::GetCurrentDirectoryFolder_RETA() + lpFolder).c_str(), NULL);

	RawLoadScript = (pLoadScript)(g_dwExeBase + rvaLoadScript);
	RawProcScript = (pProcScript)(g_dwExeBase + rvaProcScript);
	TDA::DetoursX::DetourAttachFunc(&RawLoadScript, ACV1ScriptLoad);
	TDA::DetoursX::DetourAttachFunc(&RawProcScript, ACV1ScriptDump);
}

//Read script without repack
VOID SetScriptHook(DWORD rvaLoadScript, DWORD rvaProcScript, LPCSTR lpFolder)
{
	//TDA::ConsoleX::SetConsole(L"ACV1ScriptHook");

	g_lpScriptHookFolder = lpFolder;

	RawLoadScript = (pLoadScript)(g_dwExeBase + rvaLoadScript);
	RawProcScript = (pProcScript)(g_dwExeBase + rvaProcScript);
	TDA::DetoursX::DetourAttachFunc(&RawLoadScript, ACV1ScriptLoad);
	TDA::DetoursX::DetourAttachFunc(&RawProcScript, ACV1ScriptHook);
}