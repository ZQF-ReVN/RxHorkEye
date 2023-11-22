#pragma once
#include <cstdint>


namespace ACV::VFS
{
	void SetDump(uint32_t fnVFSMediaRead, uint32_t fnVFSScriptRead, uint32_t fnVFSLuaRead, uint32_t fnScriptCompile);
	void SetDumpFolder(const char* cpFolder);
}