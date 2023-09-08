#pragma once
#include <Windows.h>
#include <shlobj.h>

VOID SetFileDump(LPCSTR lpFolder);
VOID SetFileExtract(LPCSTR lpFolder); 
//VOID SetFileHook(LPCSTR lpFolder, DWORD rvaLoadFile2);
VOID SetFileHook(LPCSTR lpFolder);
VOID SetScriptDump(DWORD rvaLoadScript, DWORD rvaProcScript, LPCSTR lpFolder);
VOID SetScriptHook(DWORD rvaLoadScript, DWORD rvaProcScript, LPCSTR lpFolder);
