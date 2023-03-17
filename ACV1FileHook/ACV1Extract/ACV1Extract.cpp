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

//To Find This Func Search String "Script.dat"
typedef DWORD(CDECL* pLoadScript)(DWORD dwHashHigh, DWORD dwHashLow, PBYTE* ppBuffer);
pLoadScript RawLoadScript = nullptr;

//This Func Is Called In LoadScript After fread();.
//Rawbuffer Will Be Released When The Func Is Complete By Call free();.
//LoadScript -> fopen -> fseek -> malloc -> fread -> fclose -> DecScript -> ProcessScript -> ret
typedef DWORD(CDECL* pDecScript)(PBYTE pDecBuffer, PDWORD pdwDecSize, PBYTE pRawBuffer, DWORD dwRawSize);
pDecScript RawDecScript = nullptr;

typedef DWORD(CDECL* pProcScript)(PBYTE pScriptBuffer, DWORD dwScriptSize, PDWORD pFunc, PDWORD pRetStruct);
pProcScript RawProcScript = nullptr;

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

static std::string g_strFileDumpFolder;
DWORD ACV1FileDump(LPCSTR lpRelativePath, PBYTE* ppFileBuffer, PDWORD dwFileSize, PDWORD dwUnknow)
{
	DWORD result = RawLoadFile(lpRelativePath, ppFileBuffer, dwFileSize, dwUnknow);

	if ((ppFileBuffer[0] == NULL) || (dwFileSize[0] == NULL)) return result;

	ACV1FileWrite((g_strFileDumpFolder + lpRelativePath).c_str(), ppFileBuffer, dwFileSize);

	printf("%s\n", lpRelativePath);
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
			std::cout << "Extract:" << fileName << std::endl;
		}
		else
		{
			std::cout << "Extract Failed!!" << std::endl;
		}

		size = 0;
		unknow = 0;
	}
}

//for this engine if can't find the hash value of filename in pack, will read the file in game directory.
//An error will be reported if the file does not exist in the directory either.
//So I changed the file name path so that the engine could not search for the corresponding file in pack
//and thus go to the directory to read the file
static std::string g_lpFileHookFolder;
DWORD ACV1FileHook(LPCSTR lpRelativePath, PBYTE* ppFileBuffer, PDWORD dwFileSize, PDWORD dwUnknow)
{
	DWORD isFileExist = GetFileAttributesA((g_lpFileHookFolder + lpRelativePath).c_str());

	if ((isFileExist != INVALID_FILE_ATTRIBUTES) && (isFileExist != FILE_ATTRIBUTE_DIRECTORY))
	{
		return RawLoadFile((g_lpFileHookFolder + lpRelativePath).c_str(), ppFileBuffer, dwFileSize, dwUnknow);
	}

	return RawLoadFile(lpRelativePath, ppFileBuffer, dwFileSize, dwUnknow);
}

static CHAR g_lpHashName[0x11] = { 0 };
DWORD ACV1LoadScript(DWORD dwHashHigh, DWORD dwHashLow, PBYTE* ppBuffer)
{
	sprintf_s(g_lpHashName, 0x11, "%X%X", dwHashHigh, dwHashLow);
	return RawLoadScript(dwHashHigh, dwHashLow, ppBuffer);
}

static std::string g_strScriptExtractFolder;
DWORD ACV1ExtractScript(PBYTE pScriptBuffer, DWORD dwScriptSize, PDWORD pFunc, PDWORD pRetStruct)
{
	HANDLE hFile = CreateFileA((g_strScriptExtractFolder + g_lpHashName).c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		WriteFile(hFile, pScriptBuffer, dwScriptSize, NULL, NULL);
		FlushFileBuffers(hFile);
		CloseHandle(hFile);
		printf("%s\n", g_lpHashName);
		memset(g_lpHashName, 0, 0x11);
	}

	return RawProcScript(pScriptBuffer, dwScriptSize, pFunc, pRetStruct);
}

static std::string g_strScriptHookFolder;
DWORD ACV1HookScript(PBYTE pScriptBuffer, DWORD dwScriptSize, PDWORD pFunc, PDWORD pRetStruct)
{
	if (g_lpHashName[0])
	{
		HANDLE hFile = CreateFileA((g_strScriptHookFolder + g_lpHashName).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD size = GetFileSize(hFile, NULL);
			PBYTE alloc = (PBYTE)VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			DWORD ret = 0;

			if (size && alloc && ReadFile(hFile, alloc, size, NULL, NULL))
			{
				ret = RawProcScript(alloc, size, pFunc, pRetStruct);
				VirtualFree(alloc, NULL, MEM_RELEASE);
			}

			memset(g_lpHashName, 0x0, 0x11);
			CloseHandle(hFile);
			return ret;
		}
	}

	return RawProcScript(pScriptBuffer, dwScriptSize, pFunc, pRetStruct);
}


//Extracting files from a running game
VOID SetFileDump(LPCSTR lpFolder)
{
	TDA::ConsoleX::SetConsole(L"ACV1FileDump");

	g_strFileDumpFolder = lpFolder;

	g_fpFileNameList = _fsopen("ACV1FileNameList.txt", "a", _SH_DENYNO);
	if (!g_fpFileNameList) MessageBoxW(NULL, L"CreateListFile Failed!!", NULL, NULL);

	TDA::DetoursX::DetourAttachFunc(&RawLoadFile, ACV1FileDump);
}

//Extracting files by filename
VOID SetFileExtract(LPCSTR lpFolder)
{
	TDA::ConsoleX::SetConsole(L"ACV1FileExtract");

	g_strFileExtractFolder = lpFolder;

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ACV1FileExtract, NULL, NULL, NULL);
}

//Read files without repack
VOID SetFileHook(LPCSTR lpFolder)
{
	g_lpFileHookFolder = lpFolder;
	CreateDirectoryA(lpFolder, NULL);
	TDA::DetoursX::DetourAttachFunc(&RawLoadFile, ACV1FileHook);
}

VOID SetScriptDump(DWORD rvaLoadScript, DWORD rvaProcScript, LPCSTR lpFolder)
{
	TDA::ConsoleX::SetConsole(L"ACV1ScriptDump");

	g_strScriptExtractFolder = lpFolder;

	SHCreateDirectoryExA(NULL, (TDA::FileX::GetCurrentDirectoryFolder_RETA() + lpFolder).c_str(), NULL);

	RawLoadScript = (pLoadScript)(g_dwExeBase + rvaLoadScript);
	RawProcScript = (pProcScript)(g_dwExeBase + rvaProcScript);
	TDA::DetoursX::DetourAttachFunc(&RawLoadScript, ACV1LoadScript);
	TDA::DetoursX::DetourAttachFunc(&RawProcScript, ACV1ExtractScript);
}

VOID SetScriptHook(DWORD rvaLoadScript, DWORD rvaProcScript, LPCSTR lpFolder)
{
	//TDA::ConsoleX::SetConsole(L"ACV1ScriptHook");

	g_strScriptHookFolder = lpFolder;

	RawLoadScript = (pLoadScript)(g_dwExeBase + rvaLoadScript);
	RawProcScript = (pProcScript)(g_dwExeBase + rvaProcScript);
	TDA::DetoursX::DetourAttachFunc(&RawLoadScript, ACV1LoadScript);
	TDA::DetoursX::DetourAttachFunc(&RawProcScript, ACV1HookScript);
}