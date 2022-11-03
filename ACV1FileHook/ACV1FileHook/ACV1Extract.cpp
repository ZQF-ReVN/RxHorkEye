#include "ACV1Extract.h"

FILE* g_fpFileNameList = 0;
CHAR g_lpHashName[0x11] = { 0 };
std::string g_strCurrentPath;
std::string g_strDumpFolder = "\\Dump\\";
std::string g_strExtractFolder = "\\Extract\\";
std::string g_strFileHookFolder = ".\\FileHook\\";
std::string g_strScriptHookFolder = ".\\FileHook\\Script\\";
std::string g_strFileDumpNameList = "ACV1FileNameList.txt";

//Game Engine Export This Func
typedef DWORD(CDECL* pLoadFile)(LPCSTR lpFileName, PBYTE* FileBuffer, PDWORD dwFileSize, PDWORD dwUnknow);
pLoadFile rawLoadFile = (pLoadFile)GetProcAddress(GetModuleHandleW(NULL), "loadFile");

//To Find This Func Search String Script.dat
typedef DWORD(CDECL* pLoadScript)(DWORD dwHashHigh, DWORD dwHashLow, PBYTE* ppBuffer);
pLoadScript rawLoadScript = 0;

//This Func Is Called In LoadScript After fread();.
//Rawbuffer Will Be Released When The Func Is Complete By Call free();.
//LoadScript -> fopen -> fseek -> malloc -> fread -> fclose -> DecScript -> free
typedef DWORD(CDECL* pDecScript)(PBYTE pDecBuffer, PDWORD pdwDecSize, PBYTE pRawBuffer, DWORD dwRawSize);
pDecScript rawDecScript = 0;

VOID CreateListFile(LPCSTR lpFileNameList)
{
	g_fpFileNameList = _fsopen(lpFileNameList, "a", _SH_DENYNO);
	if (!g_fpFileNameList)
	{
		MessageBoxW(NULL, L"CreateListFile Failed!!", NULL, NULL);
	}
}

BOOL FileExistA(std::string& strFilePath)
{
	DWORD isFileExist = GetFileAttributesA((g_strFileHookFolder + strFilePath).c_str());
	if ((isFileExist != INVALID_FILE_ATTRIBUTES) && (isFileExist != FILE_ATTRIBUTE_DIRECTORY))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL ACV1FileWrite(LPCSTR lpFileName, PBYTE* FileBuffer, PDWORD dwFileSize)
{
	HANDLE hFile = 0;
	std::string filePath;
	std::string folderPath;

	filePath = BackSlash(lpFileName);
	folderPath = GetFolderPath(filePath);

	SHCreateDirectoryExA(NULL, (g_strCurrentPath + folderPath).c_str(), NULL);

	hFile = CreateFileA((g_strCurrentPath + filePath).c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		WriteFile(hFile, *FileBuffer, *dwFileSize, NULL, NULL);
		FlushFileBuffers(hFile);
		CloseHandle(hFile);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

DWORD ACV1FileDump(LPCSTR lpFileName, PBYTE* FileBuffer, PDWORD dwFileSize, PDWORD dwUnknow)
{
	DWORD result = rawLoadFile(lpFileName, FileBuffer, dwFileSize, dwUnknow);

	printf("%s\n", lpFileName);
	fprintf(g_fpFileNameList, "%s\n", lpFileName);
	fflush(g_fpFileNameList);

	if ((*FileBuffer != NULL) && (*dwFileSize != NULL))
	{
		ACV1FileWrite(lpFileName, FileBuffer, dwFileSize);
	}

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
	std::string filePath = BackSlash(lpFileName);
	if (FileExistA(filePath))
	{
		return rawLoadFile((g_strFileHookFolder.substr(2, g_strFileHookFolder.length() - 3) + '/' + filePath).c_str(), FileBuffer, dwFileSize, dwUnknow);
	}

	return rawLoadFile(lpFileName, FileBuffer, dwFileSize, dwUnknow);
}

DWORD ACV1LoadScript(DWORD dwHashHigh, DWORD dwHashLow, PBYTE* ppBuffer)
{
	memset(g_lpHashName, 0, 0x11);
	HashToString(dwHashHigh, dwHashLow, g_lpHashName);

	DWORD ret = rawLoadScript(dwHashHigh, dwHashLow, ppBuffer);
	memset(g_lpHashName, 0, 0x11);

	return ret;
}

DWORD ACV1DecScript(PBYTE pDecBuffer, PDWORD pdwDecSize, PBYTE pRawBuffer, DWORD dwRawSize)
{
	DWORD ret = rawDecScript(pDecBuffer, pdwDecSize, pRawBuffer, dwRawSize);

	if (*g_lpHashName)
	{
		HANDLE hFile = 0;
		std::string fileName = GetScriptFileName(pDecBuffer, pdwDecSize);
		std::string hashName = g_lpHashName;
		std::cout << fileName << "#" << hashName << std::endl;

		SHCreateDirectoryExA(NULL, (g_strCurrentPath + "Script\\").c_str(), NULL);

		hFile = CreateFileA((g_strCurrentPath + "Script\\" + fileName + '#' + hashName).c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			WriteFile(hFile, pDecBuffer, *pdwDecSize, NULL, NULL);
			FlushFileBuffers(hFile);
			CloseHandle(hFile);
		}
	}

	return ret;
}

DWORD ACV1RewScript(PBYTE pDecBuffer, PDWORD pdwDecSize, PBYTE pRawBuffer, DWORD dwRawSize)
{
	DWORD ret = rawDecScript(pDecBuffer, pdwDecSize, pRawBuffer, dwRawSize);

	if (*g_lpHashName)
	{
		HANDLE hFile = 0;
		DWORD fileSize = 0;
		BOOL isRead = FALSE;
		std::string fileName = GetScriptFileName(pDecBuffer, pdwDecSize);
		std::string hashName = g_lpHashName;

		hFile = CreateFileA((g_strCurrentPath + fileName + '#' + hashName).c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			fileSize = GetFileSize(hFile, NULL);

			if (fileSize && ReadFile(hFile, pDecBuffer, fileSize, NULL, NULL))
			{
				*pdwDecSize = fileSize;
			}

			CloseHandle(hFile);
		}
	}

	return ret;
}

//Extracting files from a running game
VOID SetFileDump()
{
	SetConsole(L"ACV1FileDump");

	g_strCurrentPath = GetCurrentDirectoryPath() + g_strDumpFolder;
	CreateListFile(g_strFileDumpNameList.c_str());
	DetourAttachFunc(&rawLoadFile, ACV1FileDump);
}

//Extracting files by filename
VOID SetFileExtract()
{
	SetConsole(L"ACV1FileExtract");

	g_strCurrentPath = GetCurrentDirectoryPath() + g_strExtractFolder;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ACV1FileExtract, NULL, NULL, NULL);
}

//Read files without repack
VOID SetFileHook()
{
	CreateDirectoryA(g_strFileHookFolder.c_str(), NULL);
	DetourAttachFunc(&rawLoadFile, ACV1FileHook);
}

VOID SetScriptDump(DWORD rvaLoadScript, DWORD rvaDecScript)
{
	SetConsole(L"ACV1ScriptDump");
	g_strCurrentPath = GetCurrentDirectoryPath() + g_strDumpFolder;

	rawLoadScript = (pLoadScript)((DWORD)GetModuleHandleW(NULL) + rvaLoadScript);
	rawDecScript = (pDecScript)((DWORD)GetModuleHandleW(NULL) + rvaDecScript);
	DetourAttachFunc(&rawLoadScript, ACV1LoadScript);
	DetourAttachFunc(&rawDecScript, ACV1DecScript);
}

VOID SetScriptHook(DWORD rvaLoadScript,DWORD rvaDecScript)
{
	//SetConsole(L"ACV1ScriptHook");
	g_strCurrentPath = GetCurrentDirectoryPath() + g_strScriptHookFolder;

	rawLoadScript = (pLoadScript)((DWORD)GetModuleHandleW(NULL) + rvaLoadScript); //004CE9B0
	rawDecScript = (pDecScript)((DWORD)GetModuleHandleW(NULL) + rvaDecScript); //0052AC00
	DetourAttachFunc(&rawLoadScript, ACV1LoadScript);
	DetourAttachFunc(&rawDecScript, ACV1RewScript);
}