#include <iostream>
#include <Windows.h>

unsigned int EndianSwap(unsigned int x)
{
	return (x & 0xFF000000) >> 24 | (x & 0x00FF0000) >> 8 | (x & 0x0000FF00) << 8 | (x & 0x000000FF) << 24;
}

void ScriptReame(std::wstring msFilePath)
{
	std::wcout << L"load :" << msFilePath << L'\n';

	std::wstring folder = msFilePath.substr(0, msFilePath.rfind(L"\\") + 1) + L"Rename\\";
	std::wstring hashName = msFilePath.substr(msFilePath.rfind(L"\\") + 1);

	unsigned int low = 0;
	unsigned int hight = 0;
	wchar_t newName[0xFF] = { 0 };

	swscanf_s(hashName.substr(6, 8).c_str(), L"%x", &hight);
	swscanf_s(hashName.substr(6 + 8, 8).c_str(), L"%x", &low);

	wsprintf(newName, L"%X%X", EndianSwap(hight), EndianSwap(low));
 
	CreateDirectoryW(folder.c_str(), NULL);
	CopyFileW(hashName.c_str(), (folder + newName).c_str(), FALSE);

	std::wcout << L"save :" << folder + newName << L'\n';
}

int wmain(int argc, wchar_t* argv[])
{
	for (size_t iteArg = 1; iteArg < (size_t)argc; iteArg++)
	{
		ScriptReame(argv[iteArg]);
	}

	std::wcout << L"count:" << argc - 1 << '\n';

	system("pause");
}