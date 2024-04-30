#include "gpu.h"

namespace jipu
{
namespace hpc
{

const std::vector<Counter>& GPU::counters() const
{
    return m_counters;
}

} // namespace hpc
} // namespace jipu