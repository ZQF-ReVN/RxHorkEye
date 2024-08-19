#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>


namespace ZQF::RxHorkEye
{
    class ARC
    {
    private:
        std::unordered_map<std::uint64_t, std::string> m_umpFileName;

    public:
        ARC();

    private:
        auto FindFileNameViaCrc64(const std::uint64_t nCrc) -> std::string_view;
        auto LoadFileName(const std::string_view /*msLitsFile*/);

    public:
        auto Export(const std::string_view msPackPath, const std::string_view msSaveDir) -> bool;
        auto Import(const std::string_view msDir, const std::string_view msSavePath) -> bool;
    };
}
