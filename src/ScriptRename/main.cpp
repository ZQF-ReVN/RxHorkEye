#include <iostream>
#include <Windows.h>


static uint32_t EndianSwap(const uint32_t uiValue)
{
	return (uiValue & 0xFF000000) >> 24 | (uiValue & 0x00FF0000) >> 8 | (uiValue & 0x0000FF00) << 8 | (uiValue & 0x000000FF) << 24;
}

static void ScriptReame(std::string msPath)
{
	std::cout << "load :" << msPath << '\n';

	std::string folder = msPath.substr(0, msPath.rfind("\\") + 1) + "Rename\\";
	std::string hash_name = msPath.substr(msPath.rfind("\\") + 1);

	uint32_t hash_l = 0;
	uint32_t hash_h = 0;
	char name_buf[0xFF] = { 0 };

	::sscanf_s(hash_name.substr(6, 8).c_str(), "%x", &hash_h);
	::sscanf_s(hash_name.substr(6 + 8, 8).c_str(), "%x", &hash_l);

	sprintf_s(name_buf, 0xFF, "[%08X]-[%08X]", EndianSwap(hash_h), EndianSwap(hash_l));
 
	::CreateDirectoryA(folder.c_str(), NULL);
	::CopyFileA(hash_name.c_str(), (folder + name_buf).c_str(), FALSE);

	std::cout << "save :" << folder + name_buf << '\n';
}

int main(int argc, char* argv[])
{
	for (size_t iteArg = 1; iteArg < (size_t)argc; iteArg++)
	{
		ScriptReame(argv[iteArg]);
	}

	std::cout << "count:" << argc - 1 << '\n';

	system("pause");
}