#include "ACV1Extract.h"
#include "../TDA/ConsoleX.h"
#include "../TDA/DetoursX.h"
#include "../TDA/FileX.h"
#include "../TDA/MemX.h"

static FILE* g_fpFileNameList = NULL;
static CHAR g_lpHashName[0x11] = { 0 };
static std::string g_strCurrentPath;
static std::string g_strFileHookFolder;
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

BOOL FileExistA(std::string& strFilePath)
{
	DWORD isFileExist = GetFileAttributesA((g_strFileHookFolder + strFilePath).c_str());

	if ((isFileExist != INVALID_FILE_ATTRIBUTES) && (isFileExist != FILE_ATTRIBUTE_DIRECTORY)) return TRUE;

	return FALSE;
}


BOOL ACV1FileWrite(LPCSTR lpFileName, PBYTE* FileBuffer, PDWORD dwFileSize)
{
	std::string filePath = g_strCurrentPath + TDA::FileX::BackSlash_RET(lpFileName);
	std::string fileFolder = TDA::FileX::PathRemoveFileName_RET(filePath);

	SHCreateDirectoryExA(NULL, fileFolder.c_str(), NULL);

	HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return FALSE;

	WriteFile(hFile, *FileBuffer, *dwFileSize, NULL, NULL);
	FlushFileBuffers(hFile);
	CloseHandle(hFile);

	return TRUE;
}

DWORD ACV1FileDump(LPCSTR lpFileName, PBYTE* ppFileBuffer, PDWORD dwFileSize, PDWORD dwUnknow)
{
	DWORD result = RawLoadFile(lpFileName, ppFileBuffer, dwFileSize, dwUnknow);

	if ((ppFileBuffer[0] == NULL) || (dwFileSize[0] == NULL)) return result;

	ACV1FileWrite(lpFileName, ppFileBuffer, dwFileSize);

	printf("%s\n", lpFileName);
	fprintf(g_fpFileNameList, "%s\n", lpFileName);
	fflush(g_fpFileNameList);

	return result;
}

VOID ACV1FileExtract()
{
	BOOL isDump = FALSE;
	PBYTE buffer = 0;
	DWORD size = 0;
	DWORD dwUnknow = 0;
	std::string fileName;

	while (true)
	{
		std::cout << "FileName:";
		std::cin >> fileName;

		RawLoadFile(fileName.c_str(), &buffer, &size, &dwUnknow);
		isDump = ACV1FileWrite(fileName.c_str(), &buffer, &size);
		if (isDump)
		{
			std::cout << "Extract:" << fileName << std::endl;
		}
		else
		{
			std::cout << "Extract Failed!!" << std::endl;
		}

		size = 0;
		dwUnknow = 0;
	}
}


//for this engine if can't find the hash value of filename in pack, will read the file in game directory.
//An error will be reported if the file does not exist in the directory either.
//So I changed the file name path so that the engine could not search for the corresponding file in pack
//and thus go to the directory to read the file
DWORD ACV1FileHook(LPCSTR lpFileName, PBYTE* FileBuffer, PDWORD dwFileSize, PDWORD dwUnknow)
{
	std::string filePath = TDA::FileX::BackSlash_RET(lpFileName);

	if (FileExistA(filePath))
	{
		std::string fileRelativePath = g_strFileHookFolder.substr(2, g_strFileHookFolder.length() - 3) + '/' + filePath;
		return RawLoadFile(fileRelativePath.c_str(), FileBuffer, dwFileSize, dwUnknow);
	}

	return RawLoadFile(lpFileName, FileBuffer, dwFileSize, dwUnknow);
}


DWORD ACV1LoadScript(DWORD dwHashHigh, DWORD dwHashLow, PBYTE* ppBuffer)
{
	static CHAR aHashHigh[0x9] = { 0 };
	static CHAR aHashLow[0x9] = { 0 };
	memset(g_lpHashName, 0, 0x11);
	_itoa_s(dwHashHigh, aHashHigh, 16);
	_itoa_s(dwHashLow, aHashLow, 16);
	lstrcpyA(g_lpHashName, aHashLow);
	lstrcatA(g_lpHashName, aHashHigh);
	return RawLoadScript(dwHashHigh, dwHashLow, ppBuffer);
}

DWORD ACV1ExtractScript(PBYTE pScriptBuffer, DWORD dwScriptSize, PDWORD pFunc, PDWORD pRetStruct)
{
	if (g_lpHashName[0])
	{
		std::string filePath = g_strCurrentPath + g_lpHashName;
		HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			WriteFile(hFile, pScriptBuffer, dwScriptSize, NULL, NULL);
			FlushFileBuffers(hFile);
			CloseHandle(hFile);

			printf("%s\n", filePath.c_str());
		}
	}

	return RawProcScript(pScriptBuffer, dwScriptSize, pFunc, pRetStruct);
}

DWORD ACV1HookScript(PBYTE pScriptBuffer, DWORD dwScriptSize, PDWORD pFunc, PDWORD pRetStruct)
{
	if (g_lpHashName[0])
	{
		std::string filePath = g_strCurrentPath + g_lpHashName;
		HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		memset(g_lpHashName, 0, 0x11);

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

	g_strCurrentPath = TDA::FileX::GetCurrentDirectoryFolder_RETA() + lpFolder;

	g_fpFileNameList = _fsopen("ACV1FileNameList.txt", "a", _SH_DENYNO);
	if (!g_fpFileNameList)
	{
		MessageBoxW(NULL, L"CreateListFile Failed!!", NULL, NULL);
	}

	TDA::DetoursX::DetourAttachFunc(&RawLoadFile, ACV1FileDump);
}

//Extracting files by filename
VOID SetFileExtract(LPCSTR lpFolder)
{
	TDA::ConsoleX::SetConsole(L"ACV1FileExtract");

	g_strCurrentPath = TDA::FileX::GetCurrentDirectoryFolder_RETA() + lpFolder;

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ACV1FileExtract, NULL, NULL, NULL);
}

//Read files without repack
VOID SetFileHook(LPCSTR lpFolder)
{
	g_strFileHookFolder = lpFolder;
	CreateDirectoryA(g_strFileHookFolder.c_str(), NULL);
	TDA::DetoursX::DetourAttachFunc(&RawLoadFile, ACV1FileHook);
}

VOID SetScriptDump(DWORD rvaLoadScript, DWORD rvaProcScript, LPCSTR lpFolder)
{
	TDA::ConsoleX::SetConsole(L"ACV1ScriptDump");

	g_strCurrentPath = TDA::FileX::GetCurrentDirectoryFolder_RETA() + lpFolder;
	SHCreateDirectoryExA(NULL, g_strCurrentPath.c_str(), NULL);

	RawLoadScript = (pLoadScript)(g_dwExeBase + rvaLoadScript);
	RawProcScript = (pProcScript)(g_dwExeBase + rvaProcScript);
	TDA::DetoursX::DetourAttachFunc(&RawLoadScript, ACV1LoadScript);
	TDA::DetoursX::DetourAttachFunc(&RawProcScript, ACV1ExtractScript);
}

VOID SetScriptHook(DWORD rvaLoadScript, DWORD rvaProcScript, LPCSTR lpFolder)
{
	//TDA::ConsoleX::SetConsole(L"ACV1ScriptHook");

	g_strCurrentPath = TDA::FileX::GetCurrentDirectoryFolder_RETA() + lpFolder;

	RawLoadScript = (pLoadScript)(g_dwExeBase + rvaLoadScript);
	RawProcScript = (pProcScript)(g_dwExeBase + rvaProcScript);
	TDA::DetoursX::DetourAttachFunc(&RawLoadScript, ACV1LoadScript);
	TDA::DetoursX::DetourAttachFunc(&RawProcScript, ACV1HookScript);
}