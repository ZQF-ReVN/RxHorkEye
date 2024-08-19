#pragma once
#include <span>
#include <cstdint>
#include <string_view>


namespace ZQF::RxHorkEye
{
    class Cryptor
    {
    public:
        static auto CRC64(const std::span<const char> spData) -> std::uint64_t;
        static auto DataDecViaFileName(const std::span<std::uint8_t> spData, const std::string_view msFileName) -> void;
        static auto DataEncViaFileName(const std::span<std::uint8_t> spData, const std::string_view msFileName) -> void;
        static auto DataDecViaCRC64(const std::span<std::uint8_t> /*spData*/, const std::uint64_t /*nCRC64*/) -> void;
    };
}
