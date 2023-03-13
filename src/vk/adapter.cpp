#include "adapter.h"
#include "vk/driver.h"

namespace vkt
{

Adapter::Adapter(AdapterCreateInfo info) : m_driver(info.driver), m_physicalDevice(info.physicalDevice) {}

Adapter::~Adapter() {}

std::shared_ptr<Driver> Adapter::getDriver() { return m_driver; }

VkPhysicalDevice Adapter::getPhysicalDevice() { return m_physicalDevice; }

} // namespace vkt