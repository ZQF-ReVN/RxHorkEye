#include "DetoursX.h"

#ifdef __DetoursX__

#include "../ThirdParty/detours/include/detours.h"
#pragma comment(lib,"../ThirdParty/detours/lib.X86/detours.lib")


namespace TDA
{
	BOOL DetoursX::DetourAttachFunc(PVOID ppRawFunc, PVOID pNewFunc)
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		LONG erroAttach = DetourAttach((PVOID*)ppRawFunc, pNewFunc);
		LONG erroCommit = DetourTransactionCommit();

		if (erroAttach == NO_ERROR && erroCommit == NO_ERROR) return false;

		MessageBoxW(NULL, L"DetourAttachFunc Failed!!", NULL, NULL);

		return true;
	}

	BOOL DetoursX::DetourDetachFunc(PVOID ppRawFunc, PVOID pNewFunc)
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		LONG erroDetach = DetourDetach((PVOID*)ppRawFunc, pNewFunc);
		LONG erroCommit = DetourTransactionCommit();

		if (erroDetach == NO_ERROR && erroCommit == NO_ERROR) return false;

		MessageBoxW(NULL, L"DetourDetachFunc Failed!!", NULL, NULL);

		return true;
	}

	//********************
	//* Hook CreateFontA *
	//********************

	DWORD g_dwCharSet = 0x86;
	LPCSTR g_lpFontName = "ºÚÌå";

	typedef HFONT(WINAPI* pCreateFontA)(INT, INT, INT, INT, INT, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCSTR);
	pCreateFontA rawCreateFontA = CreateFontA;

	HFONT WINAPI newCreateFontA
	(
		INT cHeight, INT cWidth, INT cEscapement, INT cOrientation, INT cWeight,
		DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut, DWORD iCharSet, DWORD iOutPrecision,
		DWORD iClipPrecision, DWORD iQuality, DWORD iPitchAndFamily, LPCSTR pszFaceName
	)
	{
		return rawCreateFontA
		(
			cHeight, cWidth, cEscapement, cOrientation, cWeight,
			bItalic, bUnderline, bStrikeOut, g_dwCharSet, iOutPrecision,
			iClipPrecision, iQuality, iPitchAndFamily, g_lpFontName
		);
	}

	BOOL DetoursX::HookCreateFontA(DWORD dwCharSet, LPCSTR lpFontName)
	{
		g_dwCharSet = dwCharSet;
		g_lpFontName = lpFontName;
		return DetourAttachFunc(&rawCreateFontA, newCreateFontA);
	}

}

#endif // __DetoursX__