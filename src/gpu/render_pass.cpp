#include "gpu/render_pass.h"
#include "gpu/driver.h"

namespace vkt
{

RenderPass::RenderPass(Device* device, RenderPassDescriptor descriptor)
    : m_device(device)
{
}

} // namespace vkt
