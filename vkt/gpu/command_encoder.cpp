#include "vkt/gpu/command_encoder.h"
#include "vkt/gpu/device.h"

namespace vkt
{

CommandEncoder::CommandEncoder(Device* device, const CommandEncoderDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace vkt