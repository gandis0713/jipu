#include "jipu/gpu/render_pass_encoder.h"

#include "jipu/gpu/command_buffer.h"
#include "jipu/gpu/pipeline.h"
#include "jipu/gpu/pipeline_layout.h"
#include "jipu/gpu/texture_view.h"

namespace jipu
{

RenderPassEncoder::RenderPassEncoder(CommandBuffer* commandBuffer, const RenderPassEncoderDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
    , m_descriptor(descriptor)
{
}

} // namespace jipu