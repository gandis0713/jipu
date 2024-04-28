#include "counter.h"

namespace jipu
{
namespace hpc
{

Counter::Counter(std::unordered_set<Type> types)
    : m_types(types)
{
}

} // namespace hpc
} // namespace jipu