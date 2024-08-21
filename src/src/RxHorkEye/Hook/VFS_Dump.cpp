#include <RxHorkEye/Hook/VFS_Dump.h>
#include <RxHorkEye/Hook/Type.h>
#include <ZxHook/SHooker.h>
#include <ZxFile/ZxFile.h>
#include <span>
#include <string_view>
#include <Windows.h>


namespace ZQF::RxHorkEye::VFS
{
    static CRC64 sg_ScriptHash{};
    static char sg_aDumpFolder[MAX_PATH]{ "./Dump/" };


	static auto __cdecl VFSNutRead_Hook(const char* cpPath, STD_String* pScriptStr_Ret) -> bool
	{
		const auto status = ZxHook::SHooker<VFSNutRead_Hook>::FnRaw(cpPath, pScriptStr_Ret);
		if (status == true)
		{
			char full_dump_path[MAX_PATH];
			strcpy_s(full_dump_path, MAX_PATH, sg_aDumpFolder);
			strcat_s(full_dump_path, MAX_PATH, cpPath);
			char* nut_script_ptr = (pScriptStr_Ret->uiLen > 15) ? (pScriptStr_Ret->pStr) : (pScriptStr_Ret->aStr);
            ZxFile::SaveDataViaPath(full_dump_path, std::span{ nut_script_ptr, pScriptStr_Ret->uiLen });
		}
		return status;
	}

	static auto __cdecl VFSMediaRead_Hook(const char* cpPath, void** ppBuffer, size_t* pSize_Ret, uint32_t* pBufferMaxSize) -> bool
	{
		const auto status = ZxHook::SHooker<VFSMediaRead_Hook>::FnRaw(cpPath, ppBuffer, pSize_Ret, pBufferMaxSize);
		if (status == true)
		{
			char full_dump_path[MAX_PATH];
			strcpy_s(full_dump_path, MAX_PATH, sg_aDumpFolder);
			strcat_s(full_dump_path, MAX_PATH, cpPath);
            ZxFile::SaveDataViaPath(full_dump_path, std::span{ reinterpret_cast<uint8_t*>(*ppBuffer), *pSize_Ret});
		}
		return status;
	}

	static auto __cdecl VFSScriptRead_Hook(std::uint32_t nHashL, std::uint32_t nHashH, std::uint32_t* pCompile) -> bool
	{
		// Save Script Hash
		sg_ScriptHash.uiL = nHashL;
		sg_ScriptHash.uiH = nHashH;
		return ZxHook::SHooker<VFSScriptRead_Hook>::FnRaw(nHashL, nHashH, pCompile);
	}

	static auto __cdecl ScriptCompile_Hook(const char* pScript, std::uint32_t nScriptBytes, Fn_ScriptCompileLine_t fnScriptCompileLine, std::uint32_t* pCompile, const char* cpScriptFolder) -> bool
	{
		if (sg_ScriptHash.uiL || sg_ScriptHash.uiH)
		{
			// Gen Script Hash Name
			char hash_str[0x20];
			::sprintf_s(hash_str, 0x20, "[%08X]-[%08X]", sg_ScriptHash.uiL, sg_ScriptHash.uiH);
			sg_ScriptHash.uiL = 0;
			sg_ScriptHash.uiH = 0;

			// Gen Script Path
			char full_dump_path[MAX_PATH];
			strcpy_s(full_dump_path, MAX_PATH, sg_aDumpFolder);
			strcat_s(full_dump_path, MAX_PATH, "script/");
			strcat_s(full_dump_path, MAX_PATH, hash_str);

			// Save Script File
            ZxFile::SaveDataViaPath(full_dump_path, std::span{ pScript, nScriptBytes });

		}
		return ZxHook::SHooker<ScriptCompile_Hook>::FnRaw(pScript, nScriptBytes, fnScriptCompileLine, pCompile, cpScriptFolder);
	}


	auto SetDump(std::uint32_t vaVFSMediaRead, std::uint32_t vaVFSScriptRead, std::uint32_t vaVFSNutRead, std::uint32_t vaScriptCompile) -> void
	{
        ::CreateDirectoryA(sg_aDumpFolder, nullptr);

        ZxHook::SHookerDetour::CommitBeg();
        ZxHook::SHooker<VFSMediaRead_Hook>::Attach(vaVFSMediaRead);
        ZxHook::SHooker<VFSScriptRead_Hook>::Attach(vaVFSScriptRead);
        ZxHook::SHooker<VFSNutRead_Hook>::Attach(vaVFSNutRead);
        ZxHook::SHooker<ScriptCompile_Hook>::Attach(vaScriptCompile);
        ZxHook::SHookerDetour::CommitEnd();
	}

	auto SetDumpFolder(const char* cpFolder) -> void
	{
		::strcpy_s(sg_aDumpFolder, MAX_PATH, cpFolder);
	}
}
