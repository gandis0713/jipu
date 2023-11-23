#include "jipu/gpu/command_encoder.h"
#include "jipu/gpu/command_buffer.h"
#include "jipu/gpu/texture.h"
#include "jipu/gpu/texture_view.h"

namespace jipu
{

CommandEncoder::CommandEncoder(CommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
{
}

CommandBuffer* CommandEncoder::getCommandBuffer() const
{
    return m_commandBuffer;
}

} // namespace jipu