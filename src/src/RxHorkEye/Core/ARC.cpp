#include <RxHorkEye/Core/ARC.h>
#include <RxHorkEye/Core/Type.h>
#include <RxHorkEye/Core/Cryptor.h>
#include <ZxMem/ZxMem.h>
#include <ZxFile/ZxFile.h>
#include <ZxFS/Core.h>
#include <ZxFS/Searcher.h>
#include <print>
#include <span>
#include <vector>
#include <ranges>


namespace ZQF::RxHorkEye
{
    ARC::ARC()
    {

    }

    auto ARC::Export(const std::string_view msPackPath, const std::string_view msSaveDir) -> bool
    {
        ZxFile ifs{ msPackPath, ZxFile::OpenMod::ReadSafe };

        std::size_t file_cnt{};
        std::uint32_t data_key{};
        {
            const auto hdr_sig = ifs.Get<std::uint32_t>();
            if (hdr_sig == ARC_HDR_V1::GetSignature())
            {
                data_key = ARC_HDR_V1::GetFileDataKey();
                file_cnt = static_cast<std::size_t>(ifs.Get<std::uint32_t>() ^ ARC_HDR_V1::GetFileCntKey());
            }
            else
            {
                file_cnt = static_cast<std::size_t>(hdr_sig ^ ARC_HDR_V0::GetFileCntKey());
                data_key = ARC_HDR_V0::GetFileCntKey();
            }

            if (file_cnt == 0) { return false; }
        }

        ZxMem index_mem{ ARC_Entry::Sizebytes() * file_cnt };
        ifs.Read(index_mem.Span());

        ZxFS::DirMake(msSaveDir, false);

        ZxMem cache;
        ZxMem cache_dec;
        std::string path_cache;
        for (auto& entry : index_mem.Span<ARC_Entry>())
        {
            const auto file_name = m_HashStrTable.Find(entry.m_nNameCrc64);
            if (file_name.empty())
            {
                std::println("extarct failed: {:018X} -> not find file name", entry.m_nNameCrc64);
            }

            entry.DecInfo(data_key, file_name);

            ifs.Seek(entry.m_nFOA, ZxFile::MoveWay::Set);
            ifs.Read(cache.Resize(entry.m_nBytesEnc).Span());

            if (entry.m_nType == 2)
            {
                Cryptor::DataDecViaCRC64(cache.Span(), entry.m_nNameCrc64);
                // ZLIB::Decompress
                std::swap(cache, cache_dec);
            }

            Cryptor::DataDecViaFileName(cache.Span(), file_name);
            cache.Save(path_cache.append(msSaveDir).append(file_name));

            path_cache.clear();
        }

        return true;
    }

    auto ARC::Import(const std::string_view msDir, const std::string_view msSavePath) -> bool
    {
        const auto file_path_list = ZxFS::Searcher::GetFilePaths(msDir, true, true);
        const auto header = ARC_HDR_V1::Make(file_path_list.size());

        ZxFile ofs{ msSavePath, ZxFile::OpenMod::WriteForce };

        // write hdr
        ofs << std::span{ &header, 1 };

        // write index
        {
            const auto index_bytes = ARC_Entry::Sizebytes() * file_path_list.size();
            ZxMem index_mem{ index_bytes };
            std::uint32_t foa = header.Sizebytes() + index_bytes;
            for (auto&& [path, entry] : std::views::zip(file_path_list, index_mem.Span<ARC_Entry>()))
            {
                const auto file_size_opt = ZxFS::FileSize(path);
                if (file_size_opt.has_value() == false) { throw std::runtime_error(std::format("RxHorkEye::ARC::Import(): get file size error -> {}", path)); }

                const auto file_size_u32 = static_cast<std::uint32_t>(*file_size_opt);

                entry.m_nNameCrc64 = Cryptor::CRC64(path);
                entry.m_nType = 1;
                entry.m_nFOA = foa;
                entry.m_nBytesEnc = file_size_u32;
                entry.m_nBytesRaw = 0;
                entry.DecInfo(header.GetFileDataKey(), path);

                foa += file_size_u32;
            }

            ofs << index_mem.Span();
        }

        // write data
        {
            ZxMem cache_mem;
            for (const auto& path : file_path_list)
            {
                cache_mem.Load(path);
                Cryptor::DataEncViaFileName(cache_mem.Span(), path);
                ofs << cache_mem.Span();
            }
        }

        return true;
    }
}
