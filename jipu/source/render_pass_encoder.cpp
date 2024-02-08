#include "jipu/render_pass_encoder.h"

#include "jipu/command_buffer.h"
#include "jipu/pipeline.h"
#include "jipu/pipeline_layout.h"
#include "jipu/texture_view.h"

namespace jipu
{

RenderPassEncoder::RenderPassEncoder(CommandBuffer* commandBuffer, const RenderPassDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
    , m_descriptor(descriptor)
{
}

RenderPassEncoder::RenderPassEncoder(CommandBuffer* commandBuffer, const std::vector<RenderPassEncoderDescriptor>& descriptors)
    : m_commandBuffer(commandBuffer)
    , m_descriptors(descriptors)
{
}

} // namespace jipu