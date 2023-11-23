#include "jipu/command_encoder.h"
#include "jipu/command_buffer.h"
#include "jipu/texture.h"
#include "jipu/texture_view.h"

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