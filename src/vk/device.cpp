#include "device.h"

namespace vkt
{

Device::Device(const DeviceCreateInfo& info) : m_instance(info.instance), m_handle(VK_NULL_HANDLE) {}
Device::~Device() {}
} // namespace vkt