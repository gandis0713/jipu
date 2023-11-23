#include "jipu/gpu/surface.h"
#include "jipu/gpu/driver.h"

namespace jipu
{

Surface::Surface(Driver* driver, SurfaceDescriptor descriptor)
    : m_driver(driver)
    , m_windowHandle(descriptor.windowHandle)
{
}

} // namespace jipu
