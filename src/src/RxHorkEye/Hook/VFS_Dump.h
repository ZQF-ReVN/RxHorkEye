#pragma once
#include <cstdint>


namespace ZQF::RxHorkEye::VFS
{
	auto SetDump(std::uint32_t vaVFSMediaRead, std::uint32_t vaVFSScriptRead, std::uint32_t vaVFSNutRead, std::uint32_t vaScriptCompile) -> void;
	auto SetDumpFolder(const char* cpFolder) -> void;
}
