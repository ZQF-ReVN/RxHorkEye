#include "ACV1TextEditor.h"
#include "../TDA/StringX.h"
#include "../TDA/ConsoleX.h"
#include "../TDA/FormatLine.h"

#include <array>
#include <iomanip>
#include <fstream>
#include <iostream>


namespace ACV1
{
	TextEditor::TextEditor()
	{

	}

	TextEditor::~TextEditor()
	{

	}

	bool TextEditor::ReadTransLine()
	{
		std::wifstream wifsTrans(m_wsFile + L".txt"); wifsTrans.imbue(TDA::StringX::GetCVT_UTF8());
		if (!wifsTrans.is_open()) return false;

		size_t pos = 0;
		for (std::wstring line; getline(wifsTrans, line);)
		{
			if (line.empty() || line.find(L"Count:") != 0) continue;

			if (line.size() != 14)
			{
				TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
				TDA::ConsoleX::PutConsoleW(L"Line :%s\n", line);
				TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"Wrong Length");
				return false;
			}

			if (!swscanf_s(line.c_str(), L"Count:%08d", &pos))
			{
				TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
				TDA::ConsoleX::PutConsoleW(L"Line :%s\n", line);
				TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"Read Pos");
				return false;
			}

			getline(wifsTrans, line); getline(wifsTrans, line);

			if (line.find(L"Tra:") != 0)
			{
				TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
				TDA::ConsoleX::PutConsoleW(L"Line :%s\n", line);
				TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"Read Trans Line");
				return false;
			}

			m_vecTraLine.emplace_back(std::make_pair(pos, line.substr(4)));
		}

		return true;
	}

	bool TextEditor::ReadRawLine(size_t uCodePage)
	{
		std::ifstream ifsScript(m_wsFile);
		if (!ifsScript.is_open()) return false;

		std::wstring wLine;
		for (std::string line; getline(ifsScript, line);)
		{
			TDA::StringX::StrToWStr(line, wLine, uCodePage);
			m_vecRawLine.emplace_back(std::make_pair(0, wLine));
		}

		return true;
	}

	bool TextEditor::ReplaceLine(bool isInsertName)
	{
		size_t nameLen = std::wstring::npos;
		std::wstring nameTra, nameRaw;
		for (auto& iteTra : m_vecTraLine)
		{
			if (!isInsertName)
			{
				nameLen = FindCharacterName(iteTra.second);
				if (nameLen != std::wstring::npos)
				{
					nameTra = iteTra.second.substr(0, nameLen);
					nameRaw = m_vecRawLine[iteTra.first - 1].second.substr(0, nameLen);
					if (nameTra != nameRaw)
					{
						TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
						TDA::ConsoleX::PutConsoleW(L"Line :%s\n", iteTra.second);
						TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"Name Not Match");
						return false;
					}
				}
			}

			m_vecRawLine[iteTra.first - 1].first = nameLen;
			m_vecRawLine[iteTra.first - 1].second = iteTra.second;
		}

		return true;
	}

	bool TextEditor::WriteBackLine(size_t uInsCodePage, size_t uRawCodePage)
	{
		std::ofstream ofsScript(m_wsFile + L".new", std::ios::binary);
		if (!ofsScript.is_open()) return false;

		std::size_t sizeName = 0;
		std::wstring nameUTF16, textUTF16;
		std::string nameRawEcode, textNewEcode, outLine;
		for (auto& iteLine : m_vecRawLine)
		{
			if (iteLine.first)
			{
				if (iteLine.first != std::wstring::npos)
				{
					sizeName = iteLine.first;
					textUTF16 = iteLine.second.substr(sizeName);
					nameUTF16 = iteLine.second.substr(0, sizeName);
					textNewEcode = TDA::StringX::WStrToStr(textUTF16, uInsCodePage);
					nameRawEcode = TDA::StringX::WStrToStr(nameUTF16, uRawCodePage);

					outLine = nameRawEcode + textNewEcode;
				}
				else
				{
					outLine = TDA::StringX::WStrToStr(iteLine.second, uInsCodePage);
				}
			}
			else
			{
				outLine = TDA::StringX::WStrToStr(iteLine.second, uRawCodePage);
			}

			ofsScript << outLine << '\n';
		}

		ofsScript.flush();
		return true;
	}

	bool TextEditor::Filter(std::wstring& wsString)
	{
		if (wsString.find(L"CS \"") == 0) { return true; }
		if (wsString.find(L"SELECT \"") == 0) { return true; };
		if (wsString.find(L"SELECT2 \"") == 0) { return true; };

		return false;
	}

	bool TextEditor::Filter(std::string& msString, size_t uCodePage)
	{
		std::wstring wsString;
		TDA::StringX::StrToWStr(msString, wsString, uCodePage);
		
		return Filter(wsString);
	}

	size_t TextEditor::FindCharacterName(std::wstring& wsText)
	{
		static const wchar_t* token0 = L"¡¿¡¸";
		static const wchar_t* token1 = L"¡¿£¨";
		static const wchar_t* token2 = L"¡¿¡º";

		size_t pos = 0;

		if (wsText[0] == L'¡¾')
		{
			pos = wsText.find(token0, 2);
			if (pos != std::string::npos) return pos + 1;

			pos = wsText.find(token1, 2);
			if (pos != std::string::npos) return pos + 1;

			pos = wsText.find(token2, 2);
			if (pos != std::string::npos) return pos + 1;

			TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
			TDA::ConsoleX::PutConsoleW(L"Line :%s\n", wsText.c_str());
			TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"Find Character Name");
		}

		return std::string::npos;
	}


	bool TextEditor::ExtractText(std::wstring& wsFileName, size_t uCodePage)
	{
		std::ifstream ifsRaw(wsFileName);
		std::wofstream wofsTra(wsFileName + L".txt"); wofsTra.imbue(TDA::StringX::GetCVT_UTF8());
		if (!wofsTra.is_open() || !ifsRaw.is_open()) return false;

		std::wstring wsLine;
		std::size_t countLine = 1;
		for (std::string line; getline(ifsRaw, line); countLine++)
		{
			if (line.empty()) continue;
			if ((uint8_t)line[0] < (uint8_t)0x7B)
			{
				if (!Filter(line, uCodePage)) continue;
			}

			wsLine = TDA::StringX::StrToWStr(line, uCodePage);

			wofsTra
				<< L"Count:"
				<< std::setw(0x8) << std::setfill(L'0') << countLine << L'\n';

			wofsTra
				<< L"Raw:" << wsLine << L'\n'
				<< L"Tra:" << wsLine << L"\n\n";
		}

		return true;
	}

	bool TextEditor::InsertText(std::wstring& wsFileName, size_t uInsCodePage, size_t uRawCodePage, bool isInsertName)
	{
		m_wsFile = wsFileName;
		std::vector<std::pair<std::size_t, std::wstring>>().swap(m_vecRawLine);
		std::vector<std::pair<std::size_t, std::wstring>>().swap(m_vecTraLine);

		if (!ReadTransLine())
		{
			TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
			TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"ReadTransLine");
			return false;
		}

		if (!ReadRawLine(uRawCodePage))
		{
			TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
			TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"ReadRawLine");
			return false;
		}

		if (!ReplaceLine(isInsertName))
		{
			TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
			TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"ReplaceLine");
			return false;
		}

		if (!WriteBackLine(uInsCodePage, uRawCodePage))
		{
			TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
			TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"WriteBackLine");
			return false;
		}

		return true;
	}

	bool TextEditor::ListName(std::wstring& wsFileName, std::list<std::wstring>& listName)
	{
		std::wifstream wifs(wsFileName);
		wifs.imbue(TDA::StringX::GetCVT_UTF8());
		if (!wifs.is_open()) return false;

		for (std::wstring line; getline(wifs, line);)
		{
			if (line.find(L"Raw:") == std::wstring::npos) continue;

			// erase "Raw:"
			line.erase(0, 4);

			size_t nameLen = FindCharacterName(line);
			if (nameLen == std::string::npos) continue;

			// erase ¡¾ ¡¿
			line = line.substr(1, nameLen - 2);

			size_t pox = line.find(L",");
			if (pox != std::wstring::npos)
			{
				// Get Pure Name
				line = line.substr(0, pox);
			}

			listName.emplace_back(line);
		}

		return true;
	}

	bool TextEditor::FormatLine(std::wstring& wsFileName)
	{
		std::wofstream wofsText(wsFileName + L".new");
		std::wifstream wifsText(wsFileName);
		wifsText.imbue(TDA::StringX::GetCVT_UTF8());
		wofsText.imbue(TDA::StringX::GetCVT_UTF8());
		if (!wifsText.is_open() && !wofsText.is_open()) return false;

		TDA::FormatLine formatLine(L"[n]", { L"¡£", L"£¿", L"£¬", L"¡¢" });

		std::wstring copyLine;
		for (std::wstring line; std::getline(wifsText, line);)
		{
			//Check Tra: Line
			if (line.find(L"Tra:") != 0)
			{
				wofsText << line << L'\n';
				continue;
			}

			//erase "Tra:"
			line.erase(0, 4);

			//Check Code Line
			if (Filter(line))
			{
				wofsText << L"Tra:" << line << L'\n';
				continue;
			}

			//Erase [n]
			for (; ;)
			{
				size_t pos = line.find(L"[n]");
				if (pos == std::wstring::npos) break;
				line.erase(pos, 3);
			}

			//Break Line
			size_t nameLen = FindCharacterName(line);
			if (nameLen != std::wstring::npos)
			{
				copyLine = line.substr(nameLen);
				if (formatLine.BreakLine(copyLine, 32))
				{
					line = line.substr(0, nameLen) + copyLine;
				}
			}
			else
			{
				formatLine.BreakLine(line, 32);
			}

			wofsText << L"Tra:" << line << L'\n';
		}

		wofsText.flush();

		return true;
	}

}
