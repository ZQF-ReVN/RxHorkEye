#include <Windows.h>
#include <RxHorkEye/Hook/VFS_Dump.h>


static auto StartHook(HMODULE /* hDllBase */) -> void
{
	const auto exe_handle = GetModuleHandleW(NULL);
    const auto exe_va = reinterpret_cast<std::uint32_t>(exe_handle);

    {
        using namespace ZQF::RxHorkEye;

        // 宿星のガールフレンド 芙慈子編 PKG 1.0
        // VFS::SetDump(exe_va + 0xD8640, exe_va + 0xD04D0, exe_va + 0xCC020, exe_va + 0xD0AF0);

        // 魔法少女まじかるあーりん PKG 1.0
        // VFS::SetDump(exe_va + 0xBC7D0, exe_va + 0xCC1C0, exe_va + 0xBCF20, exe_va + 0xB3B10);

        // ノラと皇女と野良猫ハート２ 体験版1.00
        VFS::SetDump(exe_va + 0xD3730, exe_va + 0xCC2D0, exe_va + 0xC7BE0, exe_va + 0xCC870);
    }

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID /* lpReserved */)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
        ::StartHook(hModule);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" VOID __declspec(dllexport) DirA() {}
