#include "vkt/gpu/render_pass_encoder.h"

#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/pipeline_layout.h"
#include "vkt/gpu/texture_view.h"

namespace vkt
{

RenderPassEncoder::RenderPassEncoder(CommandBuffer* commandBuffer, const RenderPassDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
    , m_descriptor(descriptor)
{
}

} // namespace vkt