#include "jipu/gpu/compute_pass_encoder.h"
#include "jipu/gpu/command_buffer.h"

namespace jipu
{

ComputePassEncoder::ComputePassEncoder(CommandBuffer* commandBuffer, const ComputePassEncoderDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
    , m_descriptor(descriptor)
{
}

} // namespace jipu