#include "gpu/pipeline.h"
#include "gpu/device.h"

namespace vkt
{

Pipeline::Pipeline(Device* device, PipelineCreateInfo info) : m_device(device) {}

void Pipeline::setRenderPass(RenderPass* renderPass) { m_renderPass = renderPass; }

} // namespace vkt