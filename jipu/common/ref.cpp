#include "ref.h"

namespace jipu
{

Ref::Ref()
    : m_count(0)
{
}

void Ref::increase()
{
    m_count.fetch_add(1, std::memory_order_relaxed);
}

bool Ref::decrease()
{
    return m_count.fetch_sub(1, std::memory_order_acq_rel) == 1;
}

void Ref::release()
{
    if (decrease())
    {
        delete this;
    }
}

} // namespace jipu