#pragma once
#include <cstdint>


namespace ZQF::RxHorkEye::VFS
{
	auto SetHook(std::uint32_t vaOpenFile, std::uint32_t vaScriptRead, std::uint32_t vaScriptCompile) -> void;
	auto SetHookFolder(const char* cpFolder) -> void;
}
