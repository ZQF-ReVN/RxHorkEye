#include <RxHorkEye/HashStrTable.h>


namespace ZQF::RxHorkEye
{
    HashStrTable::HashStrTable()
    {

    }

    auto HashStrTable::Load(const std::string_view)
    {

    }

    auto HashStrTable::Find(const std::uint64_t nCrc) -> std::string_view
    {
        const auto ite = m_umpFileName.find(nCrc);
        return ite != m_umpFileName.end() ? ite->second : "";
    }
}
