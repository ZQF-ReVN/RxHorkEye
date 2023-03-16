#define __DetoursX__
#ifdef __DetoursX__

#pragma once
#include <Windows.h>


namespace TDA
{
	class DetoursX
	{
	public:
		static BOOL DetourAttachFunc(PVOID ppRawFunc, PVOID pNewFunc);
		static BOOL DetourDetachFunc(PVOID ppRawFunc, PVOID pNewFunc);
		static BOOL HookCreateFontA(DWORD dwCharSet, LPCSTR lpFontName);
	private:

	};

}

#endif