#include "ACV1TextEditor.h"
#include "../TDA/StringX.h"
#include "../TDA/ConsoleX.h"

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

			m_mapTraLine[pos] = line.substr(0x4);
		}

		return true;
	}

	bool TextEditor::ReadRawLine()
	{
		std::ifstream ifsScript(m_wsFile);
		if (!ifsScript.is_open()) return false;

		for (std::string line; getline(ifsScript, line);)
		{
			m_vecRawLine.emplace_back(line);
		}

		return true;
	}

	size_t TextEditor::FindCharacterName(std::string& msText)
	{
		static const uint8_t token0[] = { 0x81,0x7A,0x81,0x75,0x00 };
		static const uint8_t token1[] = { 0x81,0x7A,0x81,0x69,0x00 };
		static const uint8_t token2[] = { 0x81,0x7A,0x81,0x77,0x00 };

		size_t pos = 0;
		if ((uint8_t)msText[0] == 0x81 && (uint8_t)msText[1] == 0x79)
		{
			pos = msText.find((char*)token0, 2);
			if (pos != std::string::npos) return pos + 2;

			pos = msText.find((char*)token1, 2);
			if (pos != std::string::npos) return pos + 2;

			pos = msText.find((char*)token2, 2);
			if (pos != std::string::npos) return pos + 2;

			TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
			TDA::ConsoleX::PutConsoleW(L"Line :%s\n", TDA::StringX::StrToWStr(msText,932).c_str());
			TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"Find Character Name");
		}

		return std::string::npos;
	}

	bool TextEditor::ReplaceLine(size_t uCodePage, bool isInsertName)
	{
		std::string traText;
		std::size_t size = std::string::npos;
		for (auto& iteTra : m_mapTraLine)
		{
			std::string& rawText = m_vecRawLine[iteTra.first - 1]; 
			if (rawText.size() < 2) continue;

			traText = TDA::StringX::WStrToStr(iteTra.second, uCodePage);

			if (!isInsertName)
			{
				size = FindCharacterName(rawText);
				if (size != std::string::npos)
				{
					traText = rawText.substr(0, size) + traText.substr(size);
				}
			}

			rawText = traText;
		}

		return true;
	}

	bool TextEditor::WriteBackLine()
	{
		std::ofstream ofsScript(m_wsFile + L".new", std::ios::binary);
		if (!ofsScript.is_open()) return false;

		for (auto& iteLine : m_vecRawLine)
		{
			ofsScript << iteLine << '\n';
		}

		ofsScript.flush();
		return true;
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
				if (line.find("CS \"") != 0 || line.find("SELECT \"") != 0) continue;
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

	bool TextEditor::InsertText(std::wstring& wsFileName, size_t uCodePage, bool isInsertName)
	{
		m_wsFile = wsFileName;
		std::vector<std::string>().swap(m_vecRawLine);
		std::map<std::size_t, std::wstring>().swap(m_mapTraLine);

		if (!ReadTransLine())
		{
			TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
			TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"ReadTransLine");
			return false;
		}

		if (!ReadRawLine())
		{
			TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
			TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"ReadRawLine");
			return false;
		}

		if (!ReplaceLine(uCodePage, isInsertName))
		{
			TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
			TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"ReplaceLine");
			return false;
		}

		if (!WriteBackLine())
		{
			TDA::ConsoleX::PutConsoleW(L"File :%s\n", m_wsFile.c_str());
			TDA::ConsoleX::PutConsoleW(L"Error:%s\n\n", L"WriteBackLine");
			return false;
		}

		return true;
	}

	bool TextEditor::GetCharactersName(std::wstring& wsFileName, std::list<std::string>& listName)
	{
		std::ifstream ifs(wsFileName);
		if (!ifs.is_open()) return false;

		size_t size = std::string::npos;
		for (std::string line; getline(ifs, line);)
		{
			if (line.size() < 2) continue;

			size = FindCharacterName(line);
			if (size == std::string::npos) continue;

			line = line.substr(2, size - 4);

			for (size_t iteChar = 0; iteChar < line.size(); iteChar++)
			{
				if ((uint8_t)line[iteChar] > 0x7F) { iteChar++; continue; }
				if ((uint8_t)line[iteChar] != 0x2C) continue;
				line = line.substr(0, iteChar);
				break;
			}

			listName.emplace_back(line);
		}

		return true;
	}
}





