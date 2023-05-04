#include "vkt/gpu/surface.h"
#include "vkt/gpu/adapter.h"

namespace vkt
{

Surface::Surface(Adapter* adapter, SurfaceDescriptor descriptor)
    : m_adapter(adapter)
    , m_windowHandle(descriptor.windowHandle)
{
}

} // namespace vkt
