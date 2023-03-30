#include "gpu/device.h"
#include "gpu/adapter.h"

namespace vkt
{

Device::Device(Adapter* adapter, DeviceDescriptor descriptor)
    : m_adapter(adapter)
{
}

} // namespace vkt
