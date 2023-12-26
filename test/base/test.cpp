
#include "test.h"

namespace jipu
{

void Test::SetUp()
{
    DriverDescriptor driverDescriptor;
    driverDescriptor.type = DriverType::kVulkan;
    m_driver = Driver::create(driverDescriptor);
    EXPECT_NE(nullptr, m_driver);

    m_physicalDevices = m_driver->getPhysicalDevices();
    EXPECT_NE(0, m_physicalDevices.size());

    PhysicalDevice* physicalDevice = m_physicalDevices[0].get();
    DeviceDescriptor deviceDescriptor{};
    m_device = physicalDevice->createDevice(deviceDescriptor);
    EXPECT_NE(nullptr, m_device);
}

void Test::TearDown()
{
    m_device.reset();
    m_physicalDevices.clear();
    m_driver.reset();
}

} // namespace jipu