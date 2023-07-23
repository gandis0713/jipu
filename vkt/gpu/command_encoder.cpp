#include "vkt/gpu/command_encoder.h"
#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/pipeline_layout.h"

namespace vkt
{

CommandEncoder::CommandEncoder(CommandBuffer* commandBuffer)
    : m_commandBuffer(commandBuffer)
{
}

CommandBuffer* CommandEncoder::getCommandBuffer() const
{
    return m_commandBuffer;
}

RenderCommandEncoder::RenderCommandEncoder(CommandBuffer* commandBuffer, const RenderCommandEncoderDescriptor& descriptor)
    : CommandEncoder(commandBuffer)
    , m_descriptor(descriptor)
{
}

BlitCommandEncoder::BlitCommandEncoder(CommandBuffer* commandBuffer, const BlitCommandEncoderDescriptor& descriptor)
    : CommandEncoder(commandBuffer)
{
}

} // namespace vkt