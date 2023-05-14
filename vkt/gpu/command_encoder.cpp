#include "vkt/gpu/command_encoder.h"
#include "vkt/gpu/command_buffer.h"

namespace vkt
{

CommandEncoder::CommandEncoder(CommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
{
}

} // namespace vkt