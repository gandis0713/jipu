#pragma once

#include <unordered_set>

namespace jipu
{
namespace hpc
{

class Counter
{
public:
    enum Type
    {
        GPUActiveCy
    };

public:
    explicit Counter(std::unordered_set<Type> types);

protected:
    std::unordered_set<Type> m_types{};
};

} // namespace hpc
} // namespace jipu