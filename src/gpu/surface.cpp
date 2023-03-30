#include "gpu/surface.h"
#include "gpu/platform.h"
namespace vkt
{

Surface::Surface(Platform* platform, SurfaceDescriptor descriptor)
    : m_platform(platform)
{
}

} // namespace vkt
