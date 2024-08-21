#pragma once
#include <cstdint>
#include <cstdio>


namespace ZQF::RxHorkEye
{
#pragma pack(push)
#pragma pack(1)
	struct CRC64
	{
		std::uint32_t uiL;
        std::uint32_t uiH;
	};

	struct STD_String
	{
		union
		{
			char aStr[16];
			char* pStr;
		};

        std::uint32_t uiLen;
        std::uint32_t uiReserve;
	};
#pragma pack(pop)


	typedef FILE* (__cdecl* Fn_VFSOpenFile_t)(const char* cpPath, std::uint32_t* pSize_Ret, CRC64* pHash_Ret);

	typedef bool (__cdecl* Fn_VFSNutRead_t)(const char* cpPath, STD_String* pScriptStr_Ret);
	typedef bool (__cdecl* Fn_VFSScriptRead_t)(std::uint32_t nHashL, std::uint32_t nHashH, std::uint32_t* pCompile);
	typedef bool (__cdecl* Fn_VFSMediaRead_t)(const char* cpPath, void** ppBuffer, std::size_t* pSize_Ret, std::uint32_t* pBufferMaxSize);

	typedef void (__cdecl* Fn_ScriptCompileLine_t)(std::uint32_t* pCompile, char* pCurStr, std::size_t nLinePos);
	typedef bool (__cdecl* Fn_ScriptCompile_t)(const char* pScript, std::uint32_t nScriptBytes, Fn_ScriptCompileLine_t fnScriptCompileLine, std::uint32_t* pCompile, const char* cpScriptFolder);
} // namespace ZQF::RxHorkEye


/* 宿星のガールフレンド 芙慈子編 PKG 1.0
   fnVFSOpenFile: 0xCE8E0
   fnVFSNutRead: 0xCC020
   fnVFSMediaRead: 0xD8640
   fnVFSScriptRead: 0xD04D0
   fnScriptCompile: 0xD0AF0
*/

/* ハナヒメ＊アブソリュート！ PKG 1.3
   fnVFSOpenFile: 0xBEB30
   fnVFSNutRead: 0xBF710
   fnVFSMediaRead: 0xBEFC0
   fnVFSScriptRead: 0xCE9B0
   fnScriptCompile: 0xB66E0
*/

/* 魔法少女まじかるあーりん PKG 1.0
   fnVFSOpenFile: 0xBC340
   fnVFSNutRead: 0xBCF20
   fnVFSMediaRead: 0xBC7D0
   fnVFSScriptRead: 0xCC1C0
   fnScriptCompile: 0xB3B10
*/

/* ノラと皇女と野良猫ハート２ 体験版1.00
   fnVFSOpenFile: 0xCA520
   fnVFSNutRead: 0xC7BE0
   fnVFSMediaRead: 0xD3730
   fnVFSScriptRead: 0xCC2D0
   fnScriptCompile: 0xCC870
*/
