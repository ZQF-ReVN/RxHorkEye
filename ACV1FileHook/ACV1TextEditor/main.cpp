#include <list>
#include <iostream>
#include <algorithm>

#include "ACV1TextEditor.h"
#include "../TDA/EnumFiles.h"


int main()
{
	char flag = 0;
	bool isInsetName = false;
	std::wstring basePathW = L".\\";
	std::size_t exCodePage = 932;
	std::size_t inCodePage = 936;

	std::wcout
		<< L"Input [ d ] to dump Text\n"
		<< L"Input [ i ] to insert Text\n"
		<< L"Input [ c ] to set CodePage\n"
		<< L"Input [ n ] to replace text with role name [default not replace role name]\n\n";

	ACV1::TextEditor editor;

	while (true)
	{
		std::wcout << L"\nInput:";
		std::cin >> flag;

		TDA::EnumFilesW enumFileW(basePathW);
		std::vector<std::wstring>& filesNameListW = enumFileW.GetCurrentFilesName();

		switch (flag)
		{
		case 'd':
		{
			for (auto& strFileNameW : filesNameListW)
			{
				if (strFileNameW.find(L".", 2) != std::wstring::npos) continue;

				if (editor.ExtractText(strFileNameW, exCodePage))
				{
					std::wcout << L"Dump:" << strFileNameW << L'\n';
				}
				else
				{
					std::wcout << L"Failed:" << strFileNameW << L"\n\n";
				}
			}
		}
		break;

		case 'i':
		{
			for (auto& strFileNameW : filesNameListW)
			{
				if (strFileNameW.find(L".", 2) != std::wstring::npos) continue;

				if (editor.InsertText(strFileNameW, inCodePage, isInsetName))
				{
					std::wcout << L"Inset:" << strFileNameW << L'\n';
				}
				else
				{
					std::wcout << L"Failed:" << strFileNameW << L"\n\n";
				}
			}
		}
		break;

		case 'c':
		{
			std::wcout << L"DumpCodePage:";
			std::cin >> exCodePage;
			std::wcout << L"InsetCodePage:";
			std::cin >> inCodePage;
		}
		break;

		case 'n':
		{
			isInsetName = true;
			std::wcout << L"Replace text with role name\n";
			break;
		}

		case 'g':
		{
			std::list<std::string> nameList;

			for (auto& strFileNameW : filesNameListW)
			{
				if (strFileNameW.find(L".", 2) != std::wstring::npos) continue;

				editor.GetCharactersName(strFileNameW, nameList);
			}

			nameList.sort();
			nameList.erase(unique(nameList.begin(), nameList.end()), nameList.end());

			system("chcp 932");
			for (auto& name : nameList)
			{
				std::cout << name << '\n';
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