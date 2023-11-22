#pragma once
#include <cstdint>


namespace ACV::VFS
{
	void SetHook(uint32_t fnOpenFile, uint32_t fnScriptRead, uint32_t fnScriptCompile);
	void SetHookFolder(const char* cpFolder);
}