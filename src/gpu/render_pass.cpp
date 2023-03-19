#include "gpu/render_pass.h"
#include "gpu/driver.h"

namespace vkt
{

RenderPass::RenderPass(Device* device, RenderPassCreateInfo info) : m_device(device) {}

} // namespace vkt