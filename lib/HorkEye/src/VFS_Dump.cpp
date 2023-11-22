#include "VFS_Dump.h"
#include "Types.h"
#include "../../Rut/RxPath.h"
#include "../../Rut/RxFile.h"
#include "../../RxHook/RxHook.h"

#include <Windows.h>


namespace HorkEye::VFS
{
	static CRC64 sg_ScriptHash = { 0 };
	static char sg_aDumpFolder[MAX_PATH] = "./Dump/";
	static Fn_VFSNutRead sg_fnVFSNutRead = nullptr;
	static Fn_VFSMediaRead sg_fnVFSMediaRead = nullptr;
	static Fn_VFSScriptRead sg_fnVFSScriptRead = nullptr;
	static Fn_ScriptCompile sg_fnScriptCompile = nullptr;


	static bool __cdecl VFSNutRead_Hook(const char* cpPath, STD_String* pScriptStr_Ret)
	{
		bool status = sg_fnVFSNutRead(cpPath, pScriptStr_Ret);
		if (status == true)
		{
			char full_dump_path[MAX_PATH];
			strcpy_s(full_dump_path, MAX_PATH, sg_aDumpFolder);
			strcat_s(full_dump_path, MAX_PATH, cpPath);
			char* nut_script_ptr = (pScriptStr_Ret->uiLen > 15) ? (pScriptStr_Ret->pStr) : (pScriptStr_Ret->aStr);
			Rut::RxFile::SaveFileViaPath(full_dump_path, nut_script_ptr, pScriptStr_Ret->uiLen);
		}
		return status;
	}

	static bool __cdecl VFSMediaRead_Hook(const char* cpPath, void** ppBuffer, size_t* pSize_Ret, uint32_t* pBufferMaxSize)
	{
		bool status = sg_fnVFSMediaRead(cpPath, ppBuffer, pSize_Ret, pBufferMaxSize);
		if (status == true)
		{
			char full_dump_path[MAX_PATH];
			strcpy_s(full_dump_path, MAX_PATH, sg_aDumpFolder);
			strcat_s(full_dump_path, MAX_PATH, cpPath);
			Rut::RxFile::SaveFileViaPath(full_dump_path, *ppBuffer, *pSize_Ret);
		}
		return status;
	}

	static bool __cdecl VFSScriptRead_Hook(uint32_t nHashL, uint32_t nHashH, uint32_t* pCompile)
	{
		// Save Script Hash
		sg_ScriptHash.uiL = nHashL;
		sg_ScriptHash.uiH = nHashH;
		return sg_fnVFSScriptRead(nHashL, nHashH, pCompile);
	}

	static bool __cdecl ScriptCompile_Hook(const char* pScript, uint32_t nScriptBytes, Fn_ScriptCompileLine fnScriptCompileLine, uint32_t* pCompile, const char* cpScriptFolder)
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
			Rut::RxFile::SaveFileViaPath(full_dump_path, (void*)pScript, nScriptBytes);

		}
		return sg_fnScriptCompile(pScript, nScriptBytes, fnScriptCompileLine, pCompile, cpScriptFolder);
	}



	void SetDump(uint32_t fnVFSMediaRead, uint32_t fnVFSScriptRead, uint32_t fnVFSNutRead, uint32_t fnScriptCompile)
	{
		Rut::RxPath::MakeDirViaPath(sg_aDumpFolder);

		sg_fnVFSMediaRead = (Fn_VFSMediaRead)fnVFSMediaRead;
		sg_fnVFSScriptRead = (Fn_VFSScriptRead)fnVFSScriptRead;
		sg_fnScriptCompile = (Fn_ScriptCompile)fnScriptCompile;
		sg_fnVFSNutRead = (Fn_VFSNutRead)fnVFSNutRead;

		Rut::RxHook::DetourAttachFunc(&sg_fnVFSMediaRead, VFSMediaRead_Hook);
		Rut::RxHook::DetourAttachFunc(&sg_fnVFSScriptRead, VFSScriptRead_Hook);
		Rut::RxHook::DetourAttachFunc(&sg_fnScriptCompile, ScriptCompile_Hook);
		Rut::RxHook::DetourAttachFunc(&sg_fnVFSNutRead, VFSNutRead_Hook);
	}

	void SetDumpFolder(const char* cpFolder)
	{
		strcpy_s(sg_aDumpFolder, MAX_PATH, cpFolder);
	}
}