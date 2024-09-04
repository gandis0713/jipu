#include "ref_counted.h"

namespace jipu
{

RefCounted::RefCounted()
    : m_count(1)
{
}

void RefCounted::addRef()
{
    m_count.fetch_add(1, std::memory_order_relaxed);
}

void RefCounted::release()
{
    delete this;
}

} // namespace jipu