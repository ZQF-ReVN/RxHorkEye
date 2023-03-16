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

bool InsetText(wstring& strFileName, unsigned int uCodePage)
{
	vector<string> scriptLineList;
	map<unsigned int, wstring> transLineMAP;

	//Get Trans Text
	wifstream transFile(strFileName + L".txt");
	transFile.imbue(StringX::GetCVT_UTF8());
	if (transFile.is_open())
	{
		unsigned int position = 0;
		for (std::wstring wLine; getline(transFile, wLine);)
		{
			if (wLine.empty())
			{
				continue;
			}

			if (wLine.find(L"Count:") == 0)
			{
				swscanf_s(wLine.c_str(), L"Count:%d", &position);
				continue;
			}

			if (wLine.find(L"Tra:") == 0)
			{
				wLine = wLine.substr(0x4);
				if (!wLine.empty())
				{
					transLineMAP[position] = wLine;
				}
			}
		}

		transFile.close();
	}
	else
	{
		return false;
	}

	//Read Script By PerLine
	ifstream scriptFile(strFileName);
	if (scriptFile.is_open())
	{
		for (std::string line; getline(scriptFile, line);)
		{
			scriptLineList.push_back(line);
		}

		scriptFile.close();
	}
	else
	{
		return false;
	}

	//Out Script As New File
	ofstream newScriptFile(strFileName + L".new", ios::binary);
	if (newScriptFile.is_open())
	{
		//Replace Trans Text
		for (auto& iteTrans : transLineMAP)
		{
			StringX::WStrToStr(iteTrans.second, scriptLineList[iteTrans.first - 1], uCodePage);
		}

		//Write Back All Line
		for (auto& iteLine : scriptLineList)
		{
			newScriptFile << iteLine << '\n';
		}

		newScriptFile.close();
	}
	else
	{
		return false;
	}

	return true;
}