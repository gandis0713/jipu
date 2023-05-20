#include "vkt/gpu/shader_module.h"
#include "vkt/gpu/device.h"

namespace vkt
{

ShaderModule::ShaderModule(Device* device, const ShaderModuleDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace vkt