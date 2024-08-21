#pragma once
#include <cstdint>


namespace ZQF::RxHorkEye::VFS
{
	auto SetExtract(std::uint32_t vaVFSMediaRead, std::uint32_t vaVFSNutRead) -> void;
	auto SetExtractFolder(const char* cpFolder) -> void;
}
