#include "ACV_VFS_Dump.h"
#include "ACV_Types.h"
#include "../../Rut/RxPath.h"
#include "../../Rut/RxFile.h"
#include "../../RxHook/RxHook.h"

#include <Windows.h>


namespace ACV::VFS
{
	static ACV_Hash sg_ScriptHash = { 0 };
	static char sg_aDumpFolder[MAX_PATH] = "./Dump/";
	static Fn_VFSMediaRead sg_fnVFSMediaRead = nullptr;
	static Fn_VFSScriptRead sg_fnVFSScriptRead = nullptr;
	static Fn_VFSLuaScriptRead sg_fnVFSLuaScriptRead = nullptr;
	static Fn_CompileScript sg_fnCompileScript = nullptr;


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
		sg_ScriptHash.uiL = nHashL;
		sg_ScriptHash.uiH = nHashH;
		bool status = sg_fnVFSScriptRead(nHashL, nHashH, pCompile); // Waitting For Call CompileScript
		return status;
	}

	static bool __cdecl CompileScript_Hook(const char* pScript, uint32_t nScriptBytes, Fn_CompileLine fnCompileLine, uint32_t* pCompile, const char* cpScriptFolder)
	{
		if (sg_ScriptHash.uiL || sg_ScriptHash.uiH)
		{
			char hash_str[0x20];
			sprintf_s(hash_str, 0x20, "[%08X]-[%08X]", sg_ScriptHash.uiL, sg_ScriptHash.uiH);
			char full_dump_path[MAX_PATH];
			strcpy_s(full_dump_path, MAX_PATH, sg_aDumpFolder);
			strcat_s(full_dump_path, MAX_PATH, "script/");
			strcat_s(full_dump_path, MAX_PATH, hash_str);
			Rut::RxFile::SaveFileViaPath(full_dump_path, (void*)pScript, nScriptBytes);
			sg_ScriptHash.uiL = 0;
			sg_ScriptHash.uiH = 0;
		}
		return sg_fnCompileScript(pScript, nScriptBytes, fnCompileLine, pCompile, cpScriptFolder);
	}

	static bool __cdecl VFSLuaScriptRead_Hook(const char* cpPath, ACV_STD_String* pScriptStr_Ret)
	{
		bool status = sg_fnVFSLuaScriptRead(cpPath, pScriptStr_Ret);
		if (status == true)
		{
			char full_dump_path[MAX_PATH];
			strcpy_s(full_dump_path, MAX_PATH, sg_aDumpFolder);
			strcat_s(full_dump_path, MAX_PATH, cpPath);
			char* lua_script_ptr = (pScriptStr_Ret->uiLen > 15) ? (pScriptStr_Ret->pStr) : (pScriptStr_Ret->aStr);
			Rut::RxFile::SaveFileViaPath(full_dump_path, lua_script_ptr, pScriptStr_Ret->uiLen);
		}
		return status;
	}



	void SetDump(uint32_t fnVFSMediaRead, uint32_t fnVFSScriptRead, uint32_t fnVFSLuaScriptRead, uint32_t fnCompileScript)
	{
		Rut::RxPath::MakeDirViaPath(sg_aDumpFolder);

		sg_fnVFSMediaRead = (Fn_VFSMediaRead)fnVFSMediaRead;
		sg_fnVFSScriptRead = (Fn_VFSScriptRead)fnVFSScriptRead;
		sg_fnCompileScript = (Fn_CompileScript)fnCompileScript;
		sg_fnVFSLuaScriptRead = (Fn_VFSLuaScriptRead)fnVFSLuaScriptRead;

		Rut::RxHook::DetourAttachFunc(&sg_fnVFSMediaRead, VFSMediaRead_Hook);
		Rut::RxHook::DetourAttachFunc(&sg_fnVFSScriptRead, VFSScriptRead_Hook);
		Rut::RxHook::DetourAttachFunc(&sg_fnCompileScript, CompileScript_Hook);
		Rut::RxHook::DetourAttachFunc(&sg_fnVFSLuaScriptRead, VFSLuaScriptRead_Hook);
	}

	void SetDumpFolder(const char* cpFolder)
	{
		strcpy_s(sg_aDumpFolder, MAX_PATH, cpFolder);
	}
}