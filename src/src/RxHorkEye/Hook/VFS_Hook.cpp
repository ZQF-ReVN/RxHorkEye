#include <RxHorkEye/Hook/VFS_Hook.h>
#include <RxHorkEye/Hook/Type.h>
#include <ZxHook/SHooker.h>
#include <Windows.h>


namespace ZQF::RxHorkEye::VFS
{
	static CRC64 sg_ScriptHash = { 0 };
	static char sg_aHookPath[MAX_PATH] = "./Hook/";


	static auto __cdecl VFSOpenFile_Hook(const char* cpPath, std::uint32_t* pSize_Ret, CRC64* pHash_Ret) -> FILE*
	{
		char search_path[MAX_PATH];
		::strcpy_s(search_path, MAX_PATH, sg_aHookPath);
		::strcat_s(search_path, MAX_PATH, cpPath);
		return ZxHook::SHooker<VFSOpenFile_Hook>::FnRaw((::GetFileAttributesA(search_path) == INVALID_FILE_ATTRIBUTES) ? cpPath : search_path, pSize_Ret, pHash_Ret);
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
			// Gen Script Hash Str
			char hash_str[0x20];
			::sprintf_s(hash_str, 0x20, "[%08X]-[%08X]", sg_ScriptHash.uiL, sg_ScriptHash.uiH);
			sg_ScriptHash.uiL = 0;
			sg_ScriptHash.uiH = 0;

			// Gen Script Path
			char full_script_path[MAX_PATH];
			::strcpy_s(full_script_path, MAX_PATH, sg_aHookPath);
			::strcat_s(full_script_path, MAX_PATH, "script/");
			::strcat_s(full_script_path, MAX_PATH, hash_str);

			// Replace Script
			if (::GetFileAttributesA(full_script_path) != INVALID_FILE_ATTRIBUTES)
			{
				// Rut::RxMem::Auto script_data{ full_script_path };
				// return ZxHook::SHooker<ScriptCompile_Hook>::FnRaw((char*)script_data.GetPtr(), script_data.GetSize(), fnScriptCompileLine, pCompile, cpScriptFolder);
			}
		}
		return ZxHook::SHooker<ScriptCompile_Hook>::FnRaw(pScript, nScriptBytes, fnScriptCompileLine, pCompile, cpScriptFolder);
	}



	auto SetHook(std::uint32_t vaOpenFile, std::uint32_t vaScriptRead, std::uint32_t vaScriptCompile) -> void
	{
        ZxHook::SHookerDetour::CommitBeg();
        ZxHook::SHooker<VFSOpenFile_Hook>::Attach(vaOpenFile);
        ZxHook::SHooker<VFSScriptRead_Hook>::Attach(vaScriptRead);
        ZxHook::SHooker<ScriptCompile_Hook>::Attach(vaScriptCompile);
        ZxHook::SHookerDetour::CommitEnd();
	}

    auto SetHookFolder(const char* cpFolder) -> void
	{
		::strcpy_s(sg_aHookPath, MAX_PATH, cpFolder);
	}
}
