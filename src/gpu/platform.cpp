#include "platform.h"

namespace vkt
{

Platform::Platform(Adapter* adapter, PlatformDescriptor descriptor) noexcept
    : m_adapter(adapter)
    , m_windowHandle(descriptor.windowHandle)
{
}

Adapter* Platform::getAdapter() const
{
    return m_adapter;
}

} // namespace vkt
