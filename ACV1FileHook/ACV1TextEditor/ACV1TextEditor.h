#pragma once
#include <string>
#include <vector>
#include <map>
#include <list>
#include <tuple>

namespace ACV1
{
	class TextEditor
	{
	private:
		std::wstring m_wsFile;
		std::vector<std::string> m_vecRawLine;
		std::map<std::size_t, std::wstring> m_mapTraLine;

	private:
		bool ReadTransLine();
		bool ReadRawLine();
		bool ReplaceLine(size_t uCodePage, bool isInsetName);
		bool WriteBackLine();
		size_t FindCharacterName(std::string& msText);

	public:
		TextEditor();
		~TextEditor();

		bool ExtractText(std::wstring& wsFileName, size_t uCodePage);
		bool InsertText(std::wstring& wsFileName, size_t uCodePage, bool isInsetName);
		bool GetCharactersName(std::wstring& wsFileName, std::list<std::string>& listName);
	};

}

