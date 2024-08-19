#pragma once
#include <cstdint>


namespace HorkEye::VFS
{
	void SetExtract(uint32_t fnVFSMediaRead, uint32_t fnVFSNutRead);
	void SetExtractFolder(const char* cpFolder);
}