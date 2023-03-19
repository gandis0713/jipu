#include "gpu/device.h"
#include "gpu/adapter.h"

namespace vkt
{

Device::Device(Adapter* adapter, DeviceCreateInfo info) : m_adapter(adapter) {}

} // namespace vkt