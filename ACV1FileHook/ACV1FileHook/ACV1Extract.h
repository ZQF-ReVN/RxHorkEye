#pragma once
#include <Windows.h>
#include <shlobj.h>

VOID SetFileDump(LPCSTR lpFolder);
VOID SetFileExtract(LPCSTR lpFolder); 
VOID SetFileHook(LPCSTR lpFolder);
VOID SetScriptDump(DWORD rvaLoadScript, DWORD rvaDecScript, LPCSTR lpFolder);
VOID SetScriptHook(DWORD rvaLoadScript, DWORD rvaDecScript, LPCSTR lpFolder);