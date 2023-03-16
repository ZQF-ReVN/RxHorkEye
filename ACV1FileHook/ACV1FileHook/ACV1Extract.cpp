#include "ACV1Extract.h"
#include "../TDA/ConsoleX.h"
#include "../TDA/DetoursX.h"
#include "../TDA/FileX.h"

static FILE* g_fpFileNameList = NULL;
static CHAR g_lpHashName[0x11] = { 0 };
static std::string g_strCurrentPath;
static std::string g_strFileHookFolder;

//Game Engine Export This Func
typedef DWORD(CDECL* pLoadFile)(LPCSTR lpFileName, PBYTE* ppFileBuffer, PDWORD dwFileSize, PDWORD dwUnknow);
pLoadFile rawLoadFile = (pLoadFile)GetProcAddress(GetModuleHandleW(NULL), "loadFile");

//To Find This Func Search String Script.dat
typedef DWORD(CDECL* pLoadScript)(DWORD dwHashHigh, DWORD dwHashLow, PBYTE* ppBuffer);
pLoadScript rawLoadScript = nullptr;

//This Func Is Called In LoadScript After fread();.
//Rawbuffer Will Be Released When The Func Is Complete By Call free();.
//LoadScript -> fopen -> fseek -> malloc -> fread -> fclose -> DecScript -> free
typedef DWORD(CDECL* pDecScript)(PBYTE pDecBuffer, PDWORD pdwDecSize, PBYTE pRawBuffer, DWORD dwRawSize);
pDecScript rawDecScript = 0;

PCHAR GetScriptFileName(PBYTE pDecBuffer, DWORD dwDecSize)
{
	static char buffer[0xFF] = { 0 };

	for (size_t ite = 0; (ite < dwDecSize) && (ite < 0x100); ite++)
	{
		//Find char ¡®*¡¯ 
		if (pDecBuffer[ite] != 0x2A) continue;
		sscanf_s((PCHAR)pDecBuffer + ite + 1, "%s", buffer, 0xFF);
		return buffer;
	}

	memset(buffer, 0, 0xFF);
	return buffer;
}

VOID HashToString(DWORD dwHashHigh, DWORD dwHashLow, PCHAR lpHashName)
{
	memset(g_lpHashName, 0, 0x11);
	static CHAR hashHighName[0x9] = { 0 };
	static CHAR hashLowName[0x9] = { 0 };
	_itoa_s(dwHashHigh, hashHighName, 16);
	_itoa_s(dwHashLow, hashLowName, 16);
	lstrcpyA(lpHashName, hashLowName);
	lstrcatA(lpHashName, hashHighName);
}

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
	DWORD result = rawLoadFile(lpFileName, ppFileBuffer, dwFileSize, dwUnknow);

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

		rawLoadFile(fileName.c_str(), &buffer, &size, &dwUnknow);
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
		return rawLoadFile(fileRelativePath.c_str(), FileBuffer, dwFileSize, dwUnknow);
	}

	return rawLoadFile(lpFileName, FileBuffer, dwFileSize, dwUnknow);
}

DWORD ACV1LoadScript(DWORD dwHashHigh, DWORD dwHashLow, PBYTE* ppBuffer)
{
	HashToString(dwHashHigh, dwHashLow, g_lpHashName);
	return rawLoadScript(dwHashHigh, dwHashLow, ppBuffer);
}

DWORD ACV1DecScript(PBYTE pDecBuffer, PDWORD pdwDecSize, PBYTE pRawBuffer, DWORD dwRawSize)
{
	DWORD ret = rawDecScript(pDecBuffer, pdwDecSize, pRawBuffer, dwRawSize);

	if (g_lpHashName[0])
	{
		std::string filePath = g_strCurrentPath + GetScriptFileName(pDecBuffer, pdwDecSize[0]) + '#' + g_lpHashName;
		HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			WriteFile(hFile, pDecBuffer, pdwDecSize[0], NULL, NULL);
			FlushFileBuffers(hFile);
			CloseHandle(hFile);

			printf("%s\n", filePath.c_str());
		}
	}

	return ret;
}

DWORD ACV1HookScript(PBYTE pDecBuffer, PDWORD pdwDecSize, PBYTE pRawBuffer, DWORD dwRawSize)
{
	DWORD ret = rawDecScript(pDecBuffer, pdwDecSize, pRawBuffer, dwRawSize);

	if (g_lpHashName[0])
	{
		std::string filePath = g_strCurrentPath + GetScriptFileName(pDecBuffer, pdwDecSize[0]) + '#' + g_lpHashName;
		HANDLE hFile = CreateFileA(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD fileSize = GetFileSize(hFile, NULL);

			if (fileSize && ReadFile(hFile, pDecBuffer, fileSize, NULL, NULL))
			{
				pdwDecSize[0] = fileSize;
			}

			CloseHandle(hFile);
		}

		memset(g_lpHashName, 0, 0x11);
	}

	return ret;
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

	TDA::DetoursX::DetourAttachFunc(&rawLoadFile, ACV1FileDump);
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
	TDA::DetoursX::DetourAttachFunc(&rawLoadFile, ACV1FileHook);
}

VOID SetScriptDump(DWORD rvaLoadScript, DWORD rvaDecScript, LPCSTR lpFolder)
{
	TDA::ConsoleX::SetConsole(L"ACV1ScriptDump");

	g_strCurrentPath = TDA::FileX::GetCurrentDirectoryFolder_RETA() + lpFolder;
	SHCreateDirectoryExA(NULL, g_strCurrentPath.c_str(), NULL);

	rawLoadScript = (pLoadScript)((DWORD)GetModuleHandleW(NULL) + rvaLoadScript);
	rawDecScript = (pDecScript)((DWORD)GetModuleHandleW(NULL) + rvaDecScript);
	TDA::DetoursX::DetourAttachFunc(&rawLoadScript, ACV1LoadScript);
	TDA::DetoursX::DetourAttachFunc(&rawDecScript, ACV1DecScript);
}

VOID SetScriptHook(DWORD rvaLoadScript,DWORD rvaDecScript, LPCSTR lpFolder)
{
	//TDA::ConsoleX::SetConsole(L"ACV1ScriptHook");
	g_strCurrentPath = TDA::FileX::GetCurrentDirectoryFolder_RETA() + lpFolder;

	rawLoadScript = (pLoadScript)((DWORD)GetModuleHandleW(NULL) + rvaLoadScript); //004CE9B0
	rawDecScript = (pDecScript)((DWORD)GetModuleHandleW(NULL) + rvaDecScript); //0052AC00

	TDA::DetoursX::DetourAttachFunc(&rawLoadScript, ACV1LoadScript);
	TDA::DetoursX::DetourAttachFunc(&rawDecScript, ACV1HookScript);
}