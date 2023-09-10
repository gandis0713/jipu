#include "vkt/gpu/command_encoder.h"
#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/texture.h"
#include "vkt/gpu/texture_view.h"

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

} // namespace vkt