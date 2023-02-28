#include "driver.h"

namespace vkt
{

Driver::Driver(Platform* platform) : m_platform(*platform)
{
    // TODO: Load Vulkan entry points.

    m_context.initialize();

    // TODO: platfrom
}

Driver::~Driver() { terminate(); }

void Driver::terminate()
{
    // TODO: platfrom

    m_context.finalize();
}

} // namespace vkt