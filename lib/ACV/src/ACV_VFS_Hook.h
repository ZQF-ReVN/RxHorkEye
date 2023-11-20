#pragma once
#include <cstdint>


namespace ACV::VFS
{
	void SetResHook(uint32_t fnVFSOpenFile, uint32_t fnVFSScriptRead, uint32_t fnCompileScript);
	void SetResHookFolder(const char* cpFolder);
}