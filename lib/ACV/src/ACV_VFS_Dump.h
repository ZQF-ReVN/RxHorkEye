#pragma once
#include <cstdint>


namespace ACV::VFS
{
	void SetDump(uint32_t fnVFSMediaRead, uint32_t fnVFSScriptRead, uint32_t fnVFSLuaScriptRead, uint32_t fnCompileScript);
	void SetDumpFolder(const char* cpFolder);
}