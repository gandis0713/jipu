#include "jipu/gpu/shader_module.h"
#include "jipu/gpu/device.h"

namespace jipu
{

ShaderModule::ShaderModule(Device* device, const ShaderModuleDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace jipu