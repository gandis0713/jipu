#include "jipu/render_pass_encoder.h"

#include "jipu/command_buffer.h"
#include "jipu/pipeline.h"
#include "jipu/pipeline_layout.h"
#include "jipu/texture_view.h"

namespace jipu
{

RenderPassEncoder::RenderPassEncoder(CommandBuffer* commandBuffer, const RenderPassEncoderDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
    , m_descriptor(descriptor)
{
}

} // namespace jipu