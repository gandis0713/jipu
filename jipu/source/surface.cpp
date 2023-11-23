#include "jipu/surface.h"
#include "jipu/driver.h"

namespace jipu
{

Surface::Surface(Driver* driver, SurfaceDescriptor descriptor)
    : m_driver(driver)
    , m_windowHandle(descriptor.windowHandle)
{
}

} // namespace jipu
