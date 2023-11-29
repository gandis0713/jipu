#include "jipu/physical_device.h"

namespace jipu
{

PhysicalDevice::PhysicalDevice(Driver* driver)
    : m_driver(driver)
{
}

Driver* PhysicalDevice::getDriver() const
{
    return m_driver;
}

} // namespace jipu
