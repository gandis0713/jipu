#include "jipu/shader_module.h"
#include "jipu/device.h"

namespace jipu
{

ShaderModule::ShaderModule(Device* device, const ShaderModuleDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace jipu