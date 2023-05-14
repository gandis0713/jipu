#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/command_encoder.h"

namespace vkt
{

CommandBuffer::CommandBuffer(CommandEncoder* encoder, const CommandBufferDescriptor& descriptor)
    : m_encoder(encoder)
{
}

} // namespace vkt