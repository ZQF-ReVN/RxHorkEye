#include <RxHorkEye/Core/Cryptor.h>


namespace ZQF::RxHorkEye
{
    static const uint32_t crc64_table[512] =
    {
     0x00000000, 0x00000000, 0xA9EA3693, 0x42F0E1EB, 0x53D46D26, 0x85E1C3D7, 0xFA3E5BB5, 0xC711223C,
     0x0E42ECDF, 0x49336645, 0xA7A8DA4C, 0x0BC387AE, 0x5D9681F9, 0xCCD2A592, 0xF47CB76A, 0x8E224479,
     0x1C85D9BE, 0x9266CC8A, 0xB56FEF2D, 0xD0962D61, 0x4F51B498, 0x17870F5D, 0xE6BB820B, 0x5577EEB6,
     0x12C73561, 0xDB55AACF, 0xBB2D03F2, 0x99A54B24, 0x41135847, 0x5EB46918, 0xE8F96ED4, 0x1C4488F3,
     0x90E185EF, 0x663D78FF, 0x390BB37C, 0x24CD9914, 0xC335E8C9, 0xE3DCBB28, 0x6ADFDE5A, 0xA12C5AC3,
     0x9EA36930, 0x2F0E1EBA, 0x37495FA3, 0x6DFEFF51, 0xCD770416, 0xAAEFDD6D, 0x649D3285, 0xE81F3C86,
     0x8C645C51, 0xF45BB475, 0x258E6AC2, 0xB6AB559E, 0xDFB03177, 0x71BA77A2, 0x765A07E4, 0x334A9649,
     0x8226B08E, 0xBD68D230, 0x2BCC861D, 0xFF9833DB, 0xD1F2DDA8, 0x388911E7, 0x7818EB3B, 0x7A79F00C,
     0x21C30BDE, 0xCC7AF1FF, 0x88293D4D, 0x8E8A1014, 0x721766F8, 0x499B3228, 0xDBFD506B, 0x0B6BD3C3,
     0x2F81E701, 0x854997BA, 0x866BD192, 0xC7B97651, 0x7C558A27, 0x00A8546D, 0xD5BFBCB4, 0x4258B586,
     0x3D46D260, 0x5E1C3D75, 0x94ACE4F3, 0x1CECDC9E, 0x6E92BF46, 0xDBFDFEA2, 0xC77889D5, 0x990D1F49,
     0x33043EBF, 0x172F5B30, 0x9AEE082C, 0x55DFBADB, 0x60D05399, 0x92CE98E7, 0xC93A650A, 0xD03E790C,
     0xB1228E31, 0xAA478900, 0x18C8B8A2, 0xE8B768EB, 0xE2F6E317, 0x2FA64AD7, 0x4B1CD584, 0x6D56AB3C,
     0xBF6062EE, 0xE374EF45, 0x168A547D, 0xA1840EAE, 0xECB40FC8, 0x66952C92, 0x455E395B, 0x2465CD79,
     0xADA7578F, 0x3821458A, 0x044D611C, 0x7AD1A461, 0xFE733AA9, 0xBDC0865D, 0x57990C3A, 0xFF3067B6,
     0xA3E5BB50, 0x711223CF, 0x0A0F8DC3, 0x33E2C224, 0xF031D676, 0xF4F3E018, 0x59DBE0E5, 0xB60301F3,
     0xEA6C212F, 0xDA050215, 0x438617BC, 0x98F5E3FE, 0xB9B84C09, 0x5FE4C1C2, 0x10527A9A, 0x1D142029,
     0xE42ECDF0, 0x93366450, 0x4DC4FB63, 0xD1C685BB, 0xB7FAA0D6, 0x16D7A787, 0x1E109645, 0x5427466C,
     0xF6E9F891, 0x4863CE9F, 0x5F03CE02, 0x0A932F74, 0xA53D95B7, 0xCD820D48, 0x0CD7A324, 0x8F72ECA3,
     0xF8AB144E, 0x0150A8DA, 0x514122DD, 0x43A04931, 0xAB7F7968, 0x84B16B0D, 0x02954FFB, 0xC6418AE6,
     0x7A8DA4C0, 0xBC387AEA, 0xD3679253, 0xFEC89B01, 0x2959C9E6, 0x39D9B93D, 0x80B3FF75, 0x7B2958D6,
     0x74CF481F, 0xF50B1CAF, 0xDD257E8C, 0xB7FBFD44, 0x271B2539, 0x70EADF78, 0x8EF113AA, 0x321A3E93,
     0x66087D7E, 0x2E5EB660, 0xCFE24BED, 0x6CAE578B, 0x35DC1058, 0xABBF75B7, 0x9C3626CB, 0xE94F945C,
     0x684A91A1, 0x676DD025, 0xC1A0A732, 0x259D31CE, 0x3B9EFC87, 0xE28C13F2, 0x9274CA14, 0xA07CF219,
     0xCBAF2AF1, 0x167FF3EA, 0x62451C62, 0x548F1201, 0x987B47D7, 0x939E303D, 0x31917144, 0xD16ED1D6,
     0xC5EDC62E, 0x5F4C95AF, 0x6C07F0BD, 0x1DBC7444, 0x9639AB08, 0xDAAD5678, 0x3FD39D9B, 0x985DB793,
     0xD72AF34F, 0x84193F60, 0x7EC0C5DC, 0xC6E9DE8B, 0x84FE9E69, 0x01F8FCB7, 0x2D14A8FA, 0x43081D5C,
     0xD9681F90, 0xCD2A5925, 0x70822903, 0x8FDAB8CE, 0x8ABC72B6, 0x48CB9AF2, 0x23564425, 0x0A3B7B19,
     0x5B4EAF1E, 0x70428B15, 0xF2A4998D, 0x32B26AFE, 0x089AC238, 0xF5A348C2, 0xA170F4AB, 0xB753A929,
     0x550C43C1, 0x3971ED50, 0xFCE67552, 0x7B810CBB, 0x06D82EE7, 0xBC902E87, 0xAF321874, 0xFE60CF6C,
     0x47CB76A0, 0xE224479F, 0xEE214033, 0xA0D4A674, 0x141F1B86, 0x67C58448, 0xBDF52D15, 0x253565A3,
     0x49899A7F, 0xAB1721DA, 0xE063ACEC, 0xE9E7C031, 0x1A5DF759, 0x2EF6E20D, 0xB3B7C1CA, 0x6C0603E6,
     0x7D3274CD, 0xF6FAE5C0, 0xD4D8425E, 0xB40A042B, 0x2EE619EB, 0x731B2617, 0x870C2F78, 0x31EBC7FC,
     0x73709812, 0xBFC98385, 0xDA9AAE81, 0xFD39626E, 0x20A4F534, 0x3A284052, 0x894EC3A7, 0x78D8A1B9,
     0x61B7AD73, 0x649C294A, 0xC85D9BE0, 0x266CC8A1, 0x3263C055, 0xE17DEA9D, 0x9B89F6C6, 0xA38D0B76,
     0x6FF541AC, 0x2DAF4F0F, 0xC61F773F, 0x6F5FAEE4, 0x3C212C8A, 0xA84E8CD8, 0x95CB1A19, 0xEABE6D33,
     0xEDD3F122, 0x90C79D3F, 0x4439C7B1, 0xD2377CD4, 0xBE079C04, 0x15265EE8, 0x17EDAA97, 0x57D6BF03,
     0xE3911DFD, 0xD9F4FB7A, 0x4A7B2B6E, 0x9B041A91, 0xB04570DB, 0x5C1538AD, 0x19AF4648, 0x1EE5D946,
     0xF156289C, 0x02A151B5, 0x58BC1E0F, 0x4051B05E, 0xA28245BA, 0x87409262, 0x0B687329, 0xC5B07389,
     0xFF14C443, 0x4B9237F0, 0x56FEF2D0, 0x0962D61B, 0xACC0A965, 0xCE73F427, 0x052A9FF6, 0x8C8315CC,
     0x5CF17F13, 0x3A80143F, 0xF51B4980, 0x7870F5D4, 0x0F251235, 0xBF61D7E8, 0xA6CF24A6, 0xFD913603,
     0x52B393CC, 0x73B3727A, 0xFB59A55F, 0x31439391, 0x0167FEEA, 0xF652B1AD, 0xA88DC879, 0xB4A25046,
     0x4074A6AD, 0xA8E6D8B5, 0xE99E903E, 0xEA16395E, 0x13A0CB8B, 0x2D071B62, 0xBA4AFD18, 0x6FF7FA89,
     0x4E364A72, 0xE1D5BEF0, 0xE7DC7CE1, 0xA3255F1B, 0x1DE22754, 0x64347D27, 0xB40811C7, 0x26C49CCC,
     0xCC10FAFC, 0x5CBD6CC0, 0x65FACC6F, 0x1E4D8D2B, 0x9FC497DA, 0xD95CAF17, 0x362EA149, 0x9BAC4EFC,
     0xC2521623, 0x158E0A85, 0x6BB820B0, 0x577EEB6E, 0x91867B05, 0x906FC952, 0x386C4D96, 0xD29F28B9,
     0xD0952342, 0xCEDBA04A, 0x797F15D1, 0x8C2B41A1, 0x83414E64, 0x4B3A639D, 0x2AAB78F7, 0x09CA8276,
     0xDED7CF9D, 0x87E8C60F, 0x773DF90E, 0xC51827E4, 0x8D03A2BB, 0x020905D8, 0x24E99428, 0x40F9E433,
     0x975E55E2, 0x2CFFE7D5, 0x3EB46371, 0x6E0F063E, 0xC48A38C4, 0xA91E2402, 0x6D600E57, 0xEBEEC5E9,
     0x991CB93D, 0x65CC8190, 0x30F68FAE, 0x273C607B, 0xCAC8D41B, 0xE02D4247, 0x6322E288, 0xA2DDA3AC,
     0x8BDB8C5C, 0xBE992B5F, 0x2231BACF, 0xFC69CAB4, 0xD80FE17A, 0x3B78E888, 0x71E5D7E9, 0x79880963,
     0x85996083, 0xF7AA4D1A, 0x2C735610, 0xB55AACF1, 0xD64D0DA5, 0x724B8ECD, 0x7FA73B36, 0x30BB6F26,
     0x07BFD00D, 0x4AC29F2A, 0xAE55E69E, 0x08327EC1, 0x546BBD2B, 0xCF235CFD, 0xFD818BB8, 0x8DD3BD16,
     0x09FD3CD2, 0x03F1F96F, 0xA0170A41, 0x41011884, 0x5A2951F4, 0x86103AB8, 0xF3C36767, 0xC4E0DB53,
     0x1B3A09B3, 0xD8A453A0, 0xB2D03F20, 0x9A54B24B, 0x48EE6495, 0x5D459077, 0xE1045206, 0x1FB5719C,
     0x1578E56C, 0x919735E5, 0xBC92D3FF, 0xD367D40E, 0x46AC884A, 0x1476F632, 0xEF46BED9, 0x568617D9,
     0xB69D5E3C, 0xE085162A, 0x1F7768AF, 0xA275F7C1, 0xE549331A, 0x6564D5FD, 0x4CA30589, 0x27943416,
     0xB8DFB2E3, 0xA9B6706F, 0x11358470, 0xEB469184, 0xEB0BDFC5, 0x2C57B3B8, 0x42E1E956, 0x6EA75253,
     0xAA188782, 0x72E3DAA0, 0x03F2B111, 0x30133B4B, 0xF9CCEAA4, 0xF7021977, 0x5026DC37, 0xB5F2F89C,
     0xA45A6B5D, 0x3BD0BCE5, 0x0DB05DCE, 0x79205D0E, 0xF78E067B, 0xBE317F32, 0x5E6430E8, 0xFCC19ED9,
     0x267CDBD3, 0x86B86ED5, 0x8F96ED40, 0xC4488F3E, 0x75A8B6F5, 0x0359AD02, 0xDC428066, 0x41A94CE9,
     0x283E370C, 0xCF8B0890, 0x81D4019F, 0x8D7BE97B, 0x7BEA5A2A, 0x4A6ACB47, 0xD2006CB9, 0x089A2AAC,
     0x3AF9026D, 0x14DEA25F, 0x931334FE, 0x562E43B4, 0x692D6F4B, 0x913F6188, 0xC0C759D8, 0xD3CF8063,
     0x34BBEEB2, 0x5DEDC41A, 0x9D51D821, 0x1F1D25F1, 0x676F8394, 0xD80C07CD, 0xCE85B507, 0x9AFCE626
    };
}// namespace ZQF::RxHorkEye


namespace ZQF::RxHorkEye
{
    auto Cryptor::CRC64(const std::span<const char> spData) -> std::uint64_t
    {
        auto hash_l{ static_cast<std::uint32_t>(-1) };
        auto hash_h{ static_cast<std::uint32_t>(-1) };
        for (const auto byte : spData)
        {
            const auto tmp_byte = static_cast<std::size_t>(static_cast<const std::uint8_t>(byte) ^ static_cast<std::uint8_t>(hash_h >> 24));
            hash_h = crc64_table[1 + 2 * tmp_byte] ^ static_cast<std::uint32_t>(((static_cast<std::uint64_t>(hash_h) << 32) | static_cast<std::uint64_t>(hash_l)) >> 24);
            hash_l = crc64_table[0 + 2 * tmp_byte] ^ (hash_l << 8);

        }
        return ~static_cast<uint64_t>(static_cast<uint64_t>(hash_l) | (static_cast<uint64_t>(hash_h) << 32));
    }

    auto Cryptor::DataDecViaFileName(const std::span<std::uint8_t> spData, const std::string_view msFileName) -> void
    {
        if (msFileName.size() <= 1) { return; }

        const auto dec_block_bytes = spData.size_bytes() / msFileName.size();
        if (dec_block_bytes <= 0) { return; }

        std::size_t dec_bytes{};
        for (std::size_t file_name_idx{}; file_name_idx < (msFileName.size() - 1); file_name_idx++)
        {
            const auto file_name_byte = static_cast<std::uint8_t>(msFileName[file_name_idx]);
            for (std::size_t block_byte_idx{}; block_byte_idx < dec_block_bytes; block_byte_idx++)
            {
                spData[dec_bytes++] ^= file_name_byte;
                if (dec_bytes >= spData.size_bytes()) { return; }
            }
        }
    }

    auto Cryptor::DataEncViaFileName(const std::span<std::uint8_t> spData, const std::string_view msFileName) -> void
    {
        Cryptor::DataDecViaFileName(spData, msFileName);
    }

    auto Cryptor::DataDecViaCRC64(const std::span<std::uint8_t> /*spData*/, const std::uint64_t /*nCRC64*/) -> void
    {
        //const auto file_enc_bytes = spData.size_bytes();
        //std::size_t ite_bytes = 0;
        //std::size_t file_bytes_align_4byte = file_enc_bytes >> 2;
        //const auto v29 = _mm_shuffle_epi32(_mm_cvtsi32_si128(uiHash_L), 0);
        //if (file_enc_bytes >> 2)
        //{
        //    if (file_bytes_align_4byte >= 8)
        //    {
        //        do
        //        {
        //            *&file_enc_data_ptr[ite_bytes] = _mm_xor_si128(*&file_enc_data_ptr[ite_bytes], v29);
        //            *&file_enc_data_ptr[ite_bytes + 4] = _mm_xor_si128(*&file_enc_data_ptr[ite_bytes + 4], v29);
        //            ite_bytes += 8;
        //        } while (ite_bytes < (file_bytes_align_4byte & 0xFFFFFFF8));
        //    }

        //    for (; ite_bytes < file_bytes_align_4byte; ++ite_bytes)
        //    {
        //        file_enc_data_ptr[ite_bytes] ^= uiHash_L;
        //    }            
        //}
    }
} // namespace ZQF::RxHorkEye
