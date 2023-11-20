#include "ACV_VFS_Hook.h"
#include "ACV_Types.h"
#include "../../Rut/RxMem.h"
#include "../../RxHook/RxHook.h"

#include <Windows.h>


namespace ACV::VFS
{
	static ACV_Hash sg_ScriptHash = { 0 };
	static char sg_aHookPath[MAX_PATH] = "./Hook/";
	static Fn_VFSOpenFile sg_fnVFSOpenFile = nullptr;
	static Fn_VFSScriptRead sg_fnVFSScriptRead = nullptr;
	static Fn_CompileScript sg_fnCompileScript = nullptr;


	static FILE* __cdecl VFSOpenFile_Hook(const char* cpPath, uint32_t* pSize_Ret, ACV_Hash* pHash_Ret)
	{
		char search_path[MAX_PATH];
		::strcpy_s(search_path, MAX_PATH, sg_aHookPath);
		::strcat_s(search_path, MAX_PATH, cpPath);
		return sg_fnVFSOpenFile((::GetFileAttributesA(search_path) == INVALID_FILE_ATTRIBUTES) ? cpPath : search_path, pSize_Ret, pHash_Ret);
	}

	static bool __cdecl VFSScriptRead_Hook(uint32_t nHashL, uint32_t nHashH, uint32_t* pCompile)
	{
		// Save Script Hash
		sg_ScriptHash.uiL = nHashL;
		sg_ScriptHash.uiH = nHashH;
		return sg_fnVFSScriptRead(nHashL, nHashH, pCompile);
	}

	static bool __cdecl CompileScript_Hook(const char* pScript, uint32_t nScriptBytes, Fn_CompileLine fnCompileLine, uint32_t* pCompile, const char* cpScriptFolder)
	{
		if (sg_ScriptHash.uiL || sg_ScriptHash.uiH)
		{
			// Gen Script Hash Str
			char hash_str[0x20];
			sprintf_s(hash_str, 0x20, "[%08X]-[%08X]", sg_ScriptHash.uiL, sg_ScriptHash.uiH);
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
				Rut::RxMem::Auto script_data{ full_script_path };
				return sg_fnCompileScript((char*)script_data.GetPtr(), script_data.GetSize(), fnCompileLine, pCompile, cpScriptFolder);
			}
		}
		return sg_fnCompileScript(pScript, nScriptBytes, fnCompileLine, pCompile, cpScriptFolder);
	}



	void SetResHook(uint32_t fnVFSOpenFile, uint32_t fnVFSScriptRead, uint32_t fnCompileScript)
	{
		sg_fnVFSOpenFile = (Fn_VFSOpenFile)fnVFSOpenFile;
		sg_fnVFSScriptRead = (Fn_VFSScriptRead)fnVFSScriptRead;
		sg_fnCompileScript = (Fn_CompileScript)fnCompileScript;

		Rut::RxHook::DetourAttachFunc(&sg_fnVFSOpenFile, VFSOpenFile_Hook);
		Rut::RxHook::DetourAttachFunc(&sg_fnVFSScriptRead, VFSScriptRead_Hook);
		Rut::RxHook::DetourAttachFunc(&sg_fnCompileScript, CompileScript_Hook);
	}

	void SetResHookFolder(const char* cpFolder)
	{
		::strcpy_s(sg_aHookPath, MAX_PATH, cpFolder);
	}
}