#include <iostream>
#include "ACV1TextEditor.h"
#include "../TDA/EnumFiles.h"
using namespace std;


int main()
{
	char flag = 0;
	wstring basePathW = L".\\";
	vector<wstring> filesNameListW;
	unsigned int dumpCodePage = 932;
	unsigned int insetCodePage = 936;

	wcout << L"Input [ d ] to dump Text" << endl;
	wcout << L"Input [ i ] to insert Text" << endl;
	wcout << L"Input [ c ] to set CodePage" << endl;

	while (true)
	{
		wcout << L"\nInput:";
		cin >> flag;

		TDA::EnumFilesW enumFileW(basePathW);
		filesNameListW = enumFileW.GetCurrentFilesName();

		switch (flag)
		{
		case 'd':
			for (auto& strFileNameW : filesNameListW)
			{
				if (strFileNameW.find(L".", 2) == wstring::npos)
				{
					if (DumpText(strFileNameW, dumpCodePage))
					{
						wcout << L"Dump:" << strFileNameW << std::endl;
					}
					else
					{
						wcout << L"Failed:" << strFileNameW << std::endl;
					}
				}
			}
			break;

		case 'i':

			for (auto& strFileNameW : filesNameListW)
			{
				if (strFileNameW.find(L".", 2) == wstring::npos)
				{
					if (InsetText(strFileNameW, insetCodePage))
					{
						wcout << L"Inset:" << strFileNameW << std::endl;
					}
					else
					{
						wcout << L"Failed:" << strFileNameW << std::endl;
					}
				}
			}
			break;

		case 'c':
			wcout << L"DumpCodePage:";
			cin >> dumpCodePage;
			wcout << L"InsetCodePage:";
			cin >> insetCodePage;
			break;

		default:
			wcout << L"Illegal instructions" << endl;
			break;
		}
	}
}