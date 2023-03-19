#include "platform.h"

namespace vkt
{

Platform::Platform(Adapter* adapter, PlatformCreateInfo info) noexcept: m_adapter(adapter), m_windowHandle(info.windowHandle)  {}

} // namespace vkt
