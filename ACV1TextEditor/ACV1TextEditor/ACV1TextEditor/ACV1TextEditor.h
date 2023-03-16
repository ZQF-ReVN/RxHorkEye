#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <codecvt>
#include <map>
#include "ConvertString.h"

bool DumpText(std::wstring& strFileNmae, unsigned int uCodePage, bool isDoubleLine);
bool InsetText(std::wstring& strFileName, unsigned int uCodePage);