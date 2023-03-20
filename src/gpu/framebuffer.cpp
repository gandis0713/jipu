#include "gpu/framebuffer.h"
#include "gpu/device.h"

namespace vkt
{

FrameBuffer::FrameBuffer(Device* device, FramebufferCreateInfo info)
    : m_device(device)
{
}

} // namespace vkt