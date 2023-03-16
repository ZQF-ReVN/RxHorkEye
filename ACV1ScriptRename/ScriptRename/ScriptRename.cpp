#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>

std::string GetScriptFileName(PBYTE pDecBuffer)
{
	CHAR buffer[0xFF] = { 0 };
	for (size_t i = 0; i < 0x100; i++)
	{
		if (pDecBuffer[i] == 0x2A)
		{
			sscanf_s((PCHAR)&pDecBuffer[i], "%s", buffer, 0xFF);
			//std::cout << buffer << std::endl;
			break;
		}
	}

	std::string fileName = buffer;
	if ((unsigned char)fileName[0] == 0x2A)
	{
		fileName = fileName.substr(1);
	}

	return fileName;
}

unsigned int EndianSwap(unsigned int x)
{
	return (x & 0xFF000000) >> 24 | (x & 0x00FF0000) >> 8 | (x & 0x0000FF00) << 8 | (x & 0x000000FF) << 24;
}

VOID HashToString(DWORD dwHashHigh, DWORD dwHashLow, PCHAR lpHashName)
{
	CHAR hashHighName[0x9] = { 0 };
	CHAR hashLowName[0x9] = { 0 };
	_itoa_s(dwHashHigh, hashHighName, 16);
	_itoa_s(dwHashLow, hashLowName, 16);
	lstrcatA(lpHashName, hashLowName);
	lstrcatA(lpHashName, hashHighName);
}

VOID ScriptReame(std::string& hashFileName)
{
	if (!hashFileName.empty())
	{
		unsigned int hashLow = 0;
		unsigned int hashHigh = 0;
		CHAR hashValue[0x11] = { 0 };
		BYTE buffer[0x100] = { 0 };

		std::string scriptFileName;
		std::string newScriptFileName;

		std::ifstream iFile(hashFileName);
		iFile.read((char*)buffer, sizeof(buffer));
		scriptFileName = GetScriptFileName(buffer);

		sscanf_s(hashFileName.substr(14, 8).c_str(), "%x", &hashLow);
		sscanf_s(hashFileName.substr(6, 8).c_str(), "%x", &hashHigh);

		hashLow = EndianSwap(hashLow);
		hashHigh = EndianSwap(hashHigh);

		HashToString(hashHigh, hashLow, hashValue);
		newScriptFileName = scriptFileName + '#' + hashValue;
		CopyFileA(hashFileName.c_str(), newScriptFileName.c_str(), FALSE);
	}
}

int main(int argc, char* argv[])
{
	std::string fileName;

	while (true)
	{
		std::cin >> fileName;
		ScriptReame(fileName);
	}
}