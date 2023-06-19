#include "vkt/gpu/command_encoder.h"
#include "vkt/gpu/command_buffer.h"

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

} // namespace vkt