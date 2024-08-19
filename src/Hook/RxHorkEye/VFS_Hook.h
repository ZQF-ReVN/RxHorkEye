#pragma once
#include <cstdint>


namespace HorkEye::VFS
{
	void SetHook(uint32_t fnOpenFile, uint32_t fnScriptRead, uint32_t fnScriptCompile);
	void SetHookFolder(const char* cpFolder);
}