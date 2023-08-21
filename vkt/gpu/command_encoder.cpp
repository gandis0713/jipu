#include "vkt/gpu/command_encoder.h"
#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/pipeline_layout.h"

namespace vkt
{

CommandEncoder::CommandEncoder(CommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
{
}

CommandBuffer* CommandEncoder::getCommandBuffer() const
{
    return m_commandBuffer;
}

RenderPassEncoder::RenderPassEncoder(CommandBuffer* commandBuffer, const RenderPassEncoderDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
    , m_descriptor(descriptor)
{
}

} // namespace vkt