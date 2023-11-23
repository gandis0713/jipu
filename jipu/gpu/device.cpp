#include "jipu/gpu/device.h"
#include "jipu/gpu/physical_device.h"

namespace jipu
{

Device::Device(PhysicalDevice* physicalDevice, DeviceDescriptor descriptor)
    : m_physicalDevice(physicalDevice)
{
}

PhysicalDevice* Device::getPhysicalDevice() const
{
    return m_physicalDevice;
}

} // namespace jipu
