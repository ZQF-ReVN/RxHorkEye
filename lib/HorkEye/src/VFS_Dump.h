#pragma once
#include <cstdint>


namespace HorkEye::VFS
{
	void SetDump(uint32_t fnVFSMediaRead, uint32_t fnVFSScriptRead, uint32_t fnVFSNutRead, uint32_t fnScriptCompile);
	void SetDumpFolder(const char* cpFolder);
}