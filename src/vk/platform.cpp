#include "platform.h"

namespace vkt
{

Platform::Platform(PlatformCreateHandles handles, PlatformCreateInfo info) noexcept
    : m_windowHandle(info.windowHandle), m_instance(handles.instance), m_physicalDevice(handles.physicalDevice)
{
}

Platform::~Platform() noexcept {}

} // namespace vkt
