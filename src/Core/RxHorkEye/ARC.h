#pragma once
#include <cstdint>
#include <string_view>
#include <RxHorkEye/HashStrTable.h>


namespace ZQF::RxHorkEye
{
    class ARC
    {
    private:
        HashStrTable m_HashStrTable;

    public:
        ARC();

    public:
        auto Export(const std::string_view msPackPath, const std::string_view msSaveDir) -> bool;
        auto Import(const std::string_view msDir, const std::string_view msSavePath) -> bool;
    };
}
