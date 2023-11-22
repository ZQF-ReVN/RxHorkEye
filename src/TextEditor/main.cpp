#include <list>
#include <iostream>
#include <algorithm>

#include "ACV1TextEditor.h"

int main()
{
	char flag = 0;
	bool isInsetName = false;
	std::size_t rawCodePage = 932;
	std::size_t insCodePage = 936;

	std::wcout
		<< L"Input [ e ] to Extract Text\n"
		<< L"Input [ i ] to Insert  Text\n"
		<< L"Input [ c ] to Set CodePage\n"
		<< L"Input [ f ] to Format Text Line\n"
		<< L"Input [ l ] to List All Character Name\n"
		<< L"Input [ n ] to Replace text with role name [default not replace role name]\n\n";

	ACV1::TextEditor editor;

	while (true)
	{
		std::wcout << L"\nInput:";
		std::cin >> flag;

		std::vector<std::wstring> fileNameList;
		TDA::EnumFiles::GetAllFileNameW(L"", fileNameList);

		switch (flag)
		{
		case 'e':
		{
			for (auto& fileName : fileNameList)
			{
				if (fileName.find(L".", 2) != std::wstring::npos) continue;

				if (editor.ExtractText(fileName, rawCodePage))
				{
					std::wcout << L"Dump:" << fileName << L'\n';
				}
				else
				{
					std::wcout << L"Failed:" << fileName << L"\n\n";
				}
			}
		}
		break;

		case 'i':
		{
			for (auto& fileName : fileNameList)
			{
				if (fileName.find(L".", 2) != std::wstring::npos) continue;

				if (editor.InsertText(fileName, insCodePage, rawCodePage, isInsetName))
				{
					std::wcout << L"Inset:" << fileName << L'\n';
				}
				else
				{
					std::wcout << L"Failed:" << fileName << L"\n\n";
				}
			}
		}
		break;

		case 'c':
		{
			std::wcout << L"Original Script CodePage:";
			std::cin >> rawCodePage;
			std::wcout << L"Insert Text CodePage:";
			std::cin >> insCodePage;
		}
		break;

		case 'n':
		{
			isInsetName = true;
			std::wcout << L"Replace text with role name\n";
			break;
		}

		case 'l':
		{
			std::list<std::wstring> nameList;

			for (auto& strFileNameW : fileNameList)
			{
				if (strFileNameW.find(L".txt") == (strFileNameW.size() - 4))
				{
					editor.ListName(strFileNameW, nameList);
				}
			}

			nameList.sort();
			nameList.erase(unique(nameList.begin(), nameList.end()), nameList.end());

			for (auto& name : nameList)
			{
				TDA::ConsoleX::PutConsoleW(L"%s\n", name.c_str());
			}
		}
		break;

		case 'f':
		{
			for (auto& fileName : fileNameList)
			{
				if (fileName.find(L".txt") == (fileName.size() - 4))
				{
					editor.FormatLine(fileName);
				}
			}
		}
		break;

		default:
		{
			std::wcout << L"Illegal instructions" << std::endl;
		}
		break;
		}
	}
}