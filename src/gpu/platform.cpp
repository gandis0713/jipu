#include "platform.h"

namespace vkt
{

Platform::Platform(PlatformCreateInfo info) noexcept: m_windowHandle(info.windowHandle)  {}

} // namespace vkt
