#include "vkt/gpu/compute_pass_encoder.h"
#include "vkt/gpu/command_buffer.h"

namespace vkt
{

ComputePassEncoder::ComputePassEncoder(CommandBuffer* commandBuffer, const ComputePassEncoderDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
    , m_descriptor(descriptor)
{
}

} // namespace vkt