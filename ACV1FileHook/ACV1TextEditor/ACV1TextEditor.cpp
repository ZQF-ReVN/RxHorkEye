#include "ACV1TextEditor.h"
#include "../TDA/StringX.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;
using namespace TDA;

bool DumpText(wstring& strFileNmae, unsigned int uCodePage)
{
	unsigned int lineCount = 1;

	ifstream scriptFile(strFileNmae);
	wofstream transFile(strFileNmae + L".txt");
	transFile.imbue(StringX::GetCVT_UTF8());
	if (transFile.is_open() && scriptFile.is_open())
	{
		wstring wLine;
		for (std::string mLine; getline(scriptFile, mLine); lineCount++)
		{
			if (mLine.empty())
			{
				continue;
			}

			if (((unsigned char)mLine[0] >= (unsigned char)0x7B) ||
				mLine.find("CS \"") == 0 ||
				mLine.find("SELECT \"") == 0
				)
			{
				StringX::StrToWStr(mLine, wLine, uCodePage);
				transFile << L"Count:" << setw(0x8) << setfill(L'0') << lineCount << endl;
				transFile << L"Raw:" << wLine << endl;
				transFile << L"Tra:" << wLine << endl << endl;
			}
		}
		scriptFile.close();
		transFile.close();
		return true;
	}
	else
	{
		return false;
	}
}

bool InsetText(wstring& strFileName, unsigned int uCodePage, bool isInsetName)
{
	vector<string> scriptLineList;
	map<unsigned int, wstring> transLineMAP;

	//Get Trans Text
	wifstream transFile(strFileName + L".txt");
	transFile.imbue(StringX::GetCVT_UTF8());
	if (!transFile.is_open()) return false;

	unsigned int position = 0;
	for (std::wstring wLine; getline(transFile, wLine);)
	{
		if (wLine.empty()) continue;
		if (wLine.find(L"Count:") != 0) continue;

		if (!swscanf_s(wLine.c_str(), L"Count:%d", &position)) return false;

		getline(transFile, wLine); getline(transFile, wLine);
		if (wLine.find(L"Tra:") != 0)
		{
			std::wcout
				<< L"Read Trans Line Error\n"
				<< L"Count:" << position << L'\n'
				<< L"No Match" << std::endl;
			return false;
		}

		wLine = wLine.substr(0x4);
		transLineMAP[position] = wLine;
	}

	//Read Script By PerLine
	ifstream scriptFile(strFileName);
	if (!scriptFile.is_open()) return false;

	for (std::string line; getline(scriptFile, line);)
	{
		scriptLineList.push_back(line);
	}

	//Out Script As New File
	ofstream newScriptFile(strFileName + L".new", ios::binary);
	if (!newScriptFile.is_open()) return false;

	//Replace Trans Text
	std::string gbkText;
	std::string sjisName;
	for (auto& iteTrans : transLineMAP)
	{
		std::wstring& unicodeText = iteTrans.second;
		std::string& sjisText = scriptLineList[iteTrans.first - 1];
		TDA::StringX::WStrToStr(unicodeText, gbkText, uCodePage);

		if ((unsigned char)sjisText[0] == 0x81 && (unsigned char)sjisText[1] == 0x79 && !isInsetName)
		{
			size_t pos = sjisText.find("zu") + 2;
			if (pos != std::string::npos)
			{
				sjisName = sjisText.substr(0, pos);
				gbkText = sjisName + gbkText.substr(pos);
			}
		}

		sjisText = gbkText;
	}

	//Write Back All Line
	for (auto& iteLine : scriptLineList)
	{
		newScriptFile << iteLine << '\n';
	}

	newScriptFile.flush();

	return true;
}