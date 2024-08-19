#include "TextEditor.h"
#include "../../Rut/RxStr.h"
#include "../../Rut/RxFile.h"
#include "../../Rut/RxConsole.h"

#include <array>
#include <iomanip>
#include <iostream>


namespace HorkEye::Text
{
	TextEditor::TextEditor()
	{

	}

	TextEditor::~TextEditor()
	{

	}

	bool TextEditor::ReadTransLine()
	{
		std::vector<std::wstring> text_list;
		{
			Rut::RxFile::Text{ m_wsFile, Rut::RIO_READ, Rut::RFM_UTF8 }.ReadAllLine(text_list);
		}

		size_t pos = 0;
		for (auto ite = text_list.begin();ite != text_list.end();ite++ )
		{
			std::wstring& line = *ite;

			if (line.empty() || line.find(L"Count:") != 0) continue;

			if (line.size() != 14)
			{
				Rut::RxConsole::PutFormat(L"File :%s\n", m_wsFile.c_str());
				Rut::RxConsole::PutFormat(L"Line :%s\n", line);
				Rut::RxConsole::PutFormat(L"Error:%s\n\n", L"Wrong Length");
				return false;
			}

			if (!swscanf_s(line.c_str(), L"Count:%08d", &pos))
			{
				Rut::RxConsole::PutFormat(L"File :%s\n", m_wsFile.c_str());
				Rut::RxConsole::PutFormat(L"Line :%s\n", line);
				Rut::RxConsole::PutFormat(L"Error:%s\n\n", L"Read Pos");
				return false;
			}

			ite += 2;
			line = *ite;

			if (line.find(L"Tra:") != 0)
			{
				Rut::RxConsole::PutFormat(L"File :%s\n", m_wsFile.c_str());
				Rut::RxConsole::PutFormat(L"Line :%s\n", line);
				Rut::RxConsole::PutFormat(L"Error:%s\n\n", L"Read Trans Line");
				return false;
			}

			m_vecTraLine.emplace_back(std::make_pair(pos, std::move(line.substr(4))));
		}

		return true;
	}

	bool TextEditor::ReadRawLine(size_t uCodePage)
	{
		std::vector<std::wstring> text_list;
		{
			Rut::RxFile::Text{ m_wsFile, Rut::RIO_READ, Rut::RFM_ANSI }.ReadAllLine(text_list);
		}

		for (auto& line : text_list)
		{
			m_vecRawLine.emplace_back(std::make_pair(0, std::move(line)));
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
						Rut::RxConsole::PutFormat(L"File :%s\n", m_wsFile.c_str());
						Rut::RxConsole::PutFormat(L"Line :%s\n", iteTra.second);
						Rut::RxConsole::PutFormat(L"Error:%s\n\n", L"Name Not Match");
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
		Rut::RxFile::Text ofs{ m_wsFile + L".new", Rut::RIO_WRITE, Rut::RFM_ANSI };

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
					textNewEcode = Rut::RxStr::ToMBCS(textUTF16, uInsCodePage);
					nameRawEcode = Rut::RxStr::ToMBCS(nameUTF16, uRawCodePage);

					outLine = nameRawEcode + textNewEcode;
				}
				else
				{
					outLine = Rut::RxStr::ToMBCS(iteLine.second, uInsCodePage);
				}
			}
			else
			{
				outLine = Rut::RxStr::ToMBCS(iteLine.second, uRawCodePage);
			}

			ofs << outLine << "\n";
		}

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
		Rut::RxStr::ToWCS(msString, wsString, uCodePage);
		
		return Filter(wsString);
	}

	size_t TextEditor::FindCharacterName(std::wstring& wsText)
	{
		static const wchar_t* token0 = L"】「";
		static const wchar_t* token1 = L"】（";
		static const wchar_t* token2 = L"】『";

		size_t pos = 0;

		if (wsText[0] == L'【')
		{
			pos = wsText.find(token0, 2);
			if (pos != std::string::npos) return pos + 1;

			pos = wsText.find(token1, 2);
			if (pos != std::string::npos) return pos + 1;

			pos = wsText.find(token2, 2);
			if (pos != std::string::npos) return pos + 1;

			Rut::RxConsole::PutFormat(L"File :%s\n", m_wsFile.c_str());
			Rut::RxConsole::PutFormat(L"Line :%s\n", wsText.c_str());
			Rut::RxConsole::PutFormat(L"Error:%s\n\n", L"Find Character Name");
		}

		return std::string::npos;
	}


	bool TextEditor::ExtractText(std::wstring& wsFileName, size_t uCodePage)
	{
		std::vector<std::string> text_list;
		{
			Rut::RxFile::Text{ m_wsFile, Rut::RIO_READ, Rut::RFM_ANSI }.ReadAllLine(text_list);
		}

		Rut::RxFile::Text ofs{ wsFileName + L".txt", Rut::RIO_WRITE, Rut::RFM_UTF8 };

		std::wstring wsLine;
		std::size_t countLine = 1;
		for (auto& line : text_list)
		{
			if (line.empty()) continue;
			if ((uint8_t)line[0] < (uint8_t)0x7B)
			{
				if (!Filter(line, uCodePage)) continue;
			}

			wsLine = Rut::RxStr::ToWCS(line, uCodePage);

			ofs << L"Count:" << Rut::RxStr::NumToStr(countLine, 16) << L'\n';

			ofs << L"Raw:" << wsLine << L"\n" << L"Tra:" << wsLine << L"\n\n";
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
			Rut::RxConsole::PutFormat(L"File :%s\n", m_wsFile.c_str());
			Rut::RxConsole::PutFormat(L"Error:%s\n\n", L"ReadTransLine");
			return false;
		}

		if (!ReadRawLine(uRawCodePage))
		{
			Rut::RxConsole::PutFormat(L"File :%s\n", m_wsFile.c_str());
			Rut::RxConsole::PutFormat(L"Error:%s\n\n", L"ReadRawLine");
			return false;
		}

		if (!ReplaceLine(isInsertName))
		{
			Rut::RxConsole::PutFormat(L"File :%s\n", m_wsFile.c_str());
			Rut::RxConsole::PutFormat(L"Error:%s\n\n", L"ReplaceLine");
			return false;
		}

		if (!WriteBackLine(uInsCodePage, uRawCodePage))
		{
			Rut::RxConsole::PutFormat(L"File :%s\n", m_wsFile.c_str());
			Rut::RxConsole::PutFormat(L"Error:%s\n\n", L"WriteBackLine");
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

			// erase 【 】
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

		TDA::FormatLine formatLine(L"[n]", { L"。", L"？", L"，", L"、" });

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
