#include "vkt/gpu/physical_device.h"

namespace vkt
{

PhysicalDevice::PhysicalDevice(Driver* driver, PhysicalDeviceDescriptor descriptor)
    : m_driver(driver)
{
}

Driver* PhysicalDevice::getDriver() const
{
    return m_driver;
}

} // namespace vkt
