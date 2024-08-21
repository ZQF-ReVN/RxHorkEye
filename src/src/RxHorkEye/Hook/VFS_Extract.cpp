#include <RxHorkEye/Hook/VFS_Extract.h>
#include <RxHorkEye/Hook/Type.h>
#include <iostream>
#include <Windows.h>


namespace ZQF::RxHorkEye::VFS
{
    static char sg_aExtractFolder[MAX_PATH]{ "./Extract/" };
    static Fn_VFSNutRead_t sg_fnVFSNutRead{};
    static Fn_VFSMediaRead_t sg_fnVFSMediaRead{};

	static auto ExtractThread() -> void
	{
		auto fn_extract_media = [](const std::string_view msPath) -> bool
			{
				std::uint32_t size = 0;
				void* buffer_ptr = nullptr;
				std::uint32_t buffer_max_size = 0;

				char full_extract_path[MAX_PATH];
				::strcpy_s(full_extract_path, MAX_PATH, sg_aExtractFolder); strcat_s(full_extract_path, MAX_PATH, msPath.data());

				if (sg_fnVFSMediaRead(msPath.data(), &buffer_ptr, &size, &buffer_max_size))
				{
					// Rut::RxFile::SaveFileViaPath(full_extract_path, buffer_ptr, size);
					return true;
				}
				return false;
			};

		auto fn_extract_nut = [](const std::string_view msPath) -> bool
			{
				STD_String std_string = { 0 };

				char full_extract_path[MAX_PATH];
				::strcpy_s(full_extract_path, MAX_PATH, sg_aExtractFolder); strcat_s(full_extract_path, MAX_PATH, msPath.data());

				if (sg_fnVFSNutRead(msPath.data(), &std_string))
				{
					// Rut::RxFile::SaveFileViaPath(full_extract_path, (std_string.uiLen > 15) ? (std_string.pStr) : (std_string.aStr), std_string.uiLen);
					return true;
				}
				return false;
			};

		std::string command;
		std::string file_path;
		while (true)
		{
			std::cout << "$>";
			std::cin >> command;
			if (command == "ext.media")
			{
				std::cout << "FilePath:";
				std::cin >> file_path;
				std::cout << fn_extract_media(file_path) ? "Success!\n" : "Failed!\n";
			}
			else if (command == "ext.nut")
			{
				std::cout << "FilePath:";
				std::cin >> file_path;
				std::cout << fn_extract_nut(file_path) ? "Success!\n" : "Failed!\n";
			}
			else if (command == "help")
			{
				std::cout << "\tcommand:ext.media\n";
				std::cout << "\tcommand:ext.nut\n\n";
			}
			std::cout << std::endl;
		}
	}


	auto SetExtract(std::uint32_t vaVFSMediaRead, std::uint32_t vaVFSNutRead) -> void
	{
		sg_fnVFSNutRead = reinterpret_cast<Fn_VFSNutRead_t>(vaVFSMediaRead);
        sg_fnVFSMediaRead = reinterpret_cast<Fn_VFSMediaRead_t>(vaVFSNutRead);

		// Rut::RxPath::MakeDirViaPath(sg_aExtractFolder);
		// Rut::RxConsole::Alloc(L"HorkEye Extract", true, false);
        ::CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(ExtractThread), NULL, NULL, NULL);
	}

	auto SetExtractFolder(const char* cpFolder) -> void
	{
		::strcpy_s(sg_aExtractFolder, MAX_PATH, cpFolder);
	}
}
