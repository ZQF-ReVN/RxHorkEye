#pragma once
#include <Windows.h>
#include <string>
#include <codecvt>

std::string WStrToStr(std::wstring& wstrString, UINT uCodePage);
std::wstring StrToWStr(std::string& strString, UINT uCodePage);
std::wstring StrToWStr_CVT(std::string& strString, unsigned int uCodePage);
std::string WStrToStr_CVT(std::wstring& wstrString, unsigned int uCodePage);
