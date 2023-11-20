#pragma once
#include <cstdint>
#include <cstdio>


namespace ACV
{
	struct ACV_Hash
	{
		uint32_t uiL;
		uint32_t uiH;
	};

	struct ACV_STD_String
	{
		union
		{
			char aStr[16];
			char* pStr;
		};

		uint32_t uiLen;
		uint32_t uiReserve;
	};


	typedef FILE* (__cdecl* Fn_VFSOpenFile)(const char* cpPath, uint32_t* pSize_Ret, ACV_Hash* pHash_Ret);
	typedef bool (__cdecl* Fn_VFSMediaRead)(const char* cpPath, void** ppBuffer, size_t* pSize_Ret, uint32_t* pBufferMaxSize);
	typedef bool (__cdecl* Fn_VFSScriptRead)(uint32_t nHashL, uint32_t nHashH, uint32_t* pCompile);
	typedef bool (__cdecl* Fn_VFSLuaScriptRead)(const char* cpPath, ACV_STD_String* pScriptStr_Ret);

	typedef void (__cdecl* Fn_CompileLine)(uint32_t* pCompile, char* pCurStr, size_t nLinePos);
	typedef bool (__cdecl* Fn_CompileScript)(const char* pScript, uint32_t nScriptBytes, Fn_CompileLine fnCompileLine, uint32_t* pCompile, const char* cpScriptFolder);
}