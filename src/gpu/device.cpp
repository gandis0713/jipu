#include "vkt/gpu/device.h"
#include "vkt/gpu/adapter.h"

namespace vkt
{

Device::Device(Adapter* adapter, DeviceDescriptor descriptor)
    : m_adapter(adapter)
{
}

Adapter* Device::getAdapter() const
{
    return m_adapter;
}

} // namespace vkt
