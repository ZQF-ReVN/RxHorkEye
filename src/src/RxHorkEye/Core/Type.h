#pragma once
#include <cstdint>


namespace ZQF::RxHorkEye
{
#pragma pack(push)
#pragma pack(1)
    struct ARC_Entry
    {
        std::uint64_t m_nNameCrc64;
        std::uint8_t m_nType;
        std::uint32_t m_nFOA;
        std::uint32_t m_nBytesEnc; // in pack sise bytes
        std::uint32_t m_nBytesRaw; // after decompress size bytes

        auto DecInfo(const std::uint32_t nKey, const std::string_view msFileName) -> bool
        {
            if (m_nType != 2)
            {
                // if we don't have a filename, cannot decrypt the entry info, so we can only return
                if (msFileName.empty())
                {
                    m_nFOA = 0; // cls foa means we can't extract this file because there is no file name to decrypt info
                    return false;
                }

                m_nFOA ^= msFileName[msFileName.size() >> 1];
                m_nBytesEnc ^= msFileName[msFileName.size() >> 2];
                m_nBytesRaw ^= msFileName[msFileName.size() >> 2];
            }

            m_nType = static_cast<std::uint8_t>(m_nType ^ m_nNameCrc64); // 0 === avi; 1 == tlg,ogg; 2 == zlib; 5 == nut;
            m_nFOA = static_cast<std::uint32_t>(m_nFOA ^ m_nNameCrc64 ^ nKey);
            m_nBytesEnc = static_cast<std::uint32_t>(m_nBytesEnc ^ m_nNameCrc64);
            m_nBytesRaw = static_cast<std::uint32_t>(m_nBytesRaw ^ m_nNameCrc64);

            return true;
        }

        static auto Sizebytes() -> std::uint32_t
        {
            return sizeof(ARC_Entry);
        }
    };

    struct ARC_HDR_V0
    {
    public:
        std::uint32_t m_nFileCnt;

    public:
        static auto GetFileCntKey() -> std::uint32_t
        {
            return 0x26ACA46E;
        }

        static auto GetFileDataKey() -> std::uint32_t
        {
            return 0x0;
        }
    };

    struct ARC_HDR_V1
    {
    public:
        std::uint32_t m_nSig;
        std::uint32_t m_nFileCnt;

    public:
        static auto GetSignature() -> std::uint32_t
        {
            return '1VCA';
        }

        static auto GetFileCntKey() -> std::uint32_t
        {
            return 0x8B6A4E5F;
        }

        static auto GetFileDataKey() -> std::uint32_t
        {
            return ARC_HDR_V1::GetFileCntKey();
        }

        static auto Sizebytes() -> std::uint32_t
        {
            return sizeof(ARC_HDR_V1);
        }

        static auto Make(const std::size_t nFileCnt) -> ARC_HDR_V1
        {
            ARC_HDR_V1 hdr
            {
                .m_nSig = ARC_HDR_V1::GetSignature(),
                .m_nFileCnt = static_cast<const std::uint32_t>(nFileCnt) ^ ARC_HDR_V1::GetFileCntKey(),
            };

            return hdr;
        }
    };
#pragma pack(pop)

}
