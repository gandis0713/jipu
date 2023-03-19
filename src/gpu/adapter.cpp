#include "gpu/adapter.h"

namespace vkt
{

Adapter::Adapter(Driver* driver, AdapterCreateInfo info) : m_driver(driver) {}

Driver* Adapter::getDriver() const { return m_driver; }

} // namespace vkt