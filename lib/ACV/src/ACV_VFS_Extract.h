#pragma once
#include <cstdint>


namespace ACV::VFS
{
	void SetExtract(uint32_t fnVFSMediaRead, uint32_t fnVFSLuaScriptRead);
	void SetExtractFolder(const char* cpFolder);
}