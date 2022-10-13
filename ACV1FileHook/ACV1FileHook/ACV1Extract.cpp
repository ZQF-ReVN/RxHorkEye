#include "ACV1Extract.h"

FILE* g_fpFileNameList = 0;
std::string g_strCurrentPath;
std::string g_strDumpFolder = "\\Dump\\";
std::string g_strExtractFolder = "\\Extract\\";
std::string g_strFileHookFolder = ".\\FileHook\\";
std::string g_strDumpFileNameList = "ACV1FileNameList.txt";

typedef DWORD(CDECL* pLoadFile)(LPCSTR lpFileName, PBYTE* FileBuffer, PDWORD dwFileSize, PDWORD dwUnknow);
pLoadFile rawLoadFile = (pLoadFile)GetProcAddress(GetModuleHandleW(NULL), "loadFile");

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

	buffer = (PBYTE)VirtualAlloc(NULL, 0x1000 * 0x2000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

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

DWORD ACV1FileHook(LPCSTR lpFileName, PBYTE* FileBuffer, PDWORD dwFileSize, PDWORD dwUnknow)
{
	std::string filePath = BackSlash(lpFileName);
	if (FileExistA(filePath))
	{
		return rawLoadFile((g_strFileHookFolder.substr(2, g_strFileHookFolder.length() - 3) + '/' + filePath).c_str(), FileBuffer, dwFileSize, dwUnknow);
	}

	return rawLoadFile(lpFileName, FileBuffer, dwFileSize, dwUnknow);
}

//Extracting files from a running game
VOID SetFileDump()
{
	SetConsole(L"ACV1Dump");

	g_strCurrentPath = GetCurrentDirectoryPath() + g_strDumpFolder;
	CreateListFile(g_strDumpFileNameList.c_str());
	DetourAttachFunc(&rawLoadFile, ACV1FileDump);
}

//Extracting files by filename
VOID SetFileExtract()
{
	SetConsole(L"ACV1Extract");

	g_strCurrentPath = GetCurrentDirectoryPath() + g_strExtractFolder;
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ACV1FileExtract, NULL, NULL, NULL);
}

//Read files without repack
VOID SetFileHook()
{
	DetourAttachFunc(&rawLoadFile, ACV1FileHook);
}