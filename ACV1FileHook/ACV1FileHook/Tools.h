#pragma once
#include <Windows.h>
#include <detours.h>
#include <shlobj.h>
#include <iostream>

FILE* SetConsole(LPCWSTR lpTitle);
std::string GetCurrentDirectoryPath();
std::string BackSlash(LPCSTR lpPath);
std::string GetFileName(std::string& strPath);
std::string GetFolderPath(std::string& strPath);
BOOL WriteMemory(LPVOID lpAddress, LPCVOID lpBuffer, SIZE_T szSize);
BOOL SetHook(DWORD dwRawAddr, DWORD dwTarAddr, SIZE_T szRawSize);
BOOL DetourAttachFunc(PVOID ppPointer, PVOID pDetour);
BOOL DetourDetachFunc(PVOID ppPointer, PVOID pDetour);