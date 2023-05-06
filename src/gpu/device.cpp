#include "vkt/gpu/device.h"
#include "vkt/gpu/physical_device.h"

namespace vkt
{

Device::Device(PhysicalDevice* physicalDevice, DeviceDescriptor descriptor)
    : m_physicalDevice(physicalDevice)
{
}

PhysicalDevice* Device::getPhysicalDevice() const
{
    return m_physicalDevice;
}

} // namespace vkt
