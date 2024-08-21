#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>


namespace ZQF::RxHorkEye
{
    class HashStrTable
    {
    private:
        std::unordered_map<std::uint64_t, std::string> m_umpFileName;

    public:
        HashStrTable();

    public:
        auto Load(const std::string_view /*msLitsFile*/);

    public:
        auto Find(const std::uint64_t nCrc) -> std::string_view;
    };
}
