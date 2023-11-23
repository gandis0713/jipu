#include "jipu/compute_pass_encoder.h"
#include "jipu/command_buffer.h"

namespace jipu
{

ComputePassEncoder::ComputePassEncoder(CommandBuffer* commandBuffer, const ComputePassEncoderDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
    , m_descriptor(descriptor)
{
}

} // namespace jipu