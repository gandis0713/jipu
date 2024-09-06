#include "webgpu_shader_module.h"

namespace jipu
{

WebGPUShaderModule* create(WebGPUDevice* device, WGPUShaderModuleDescriptor const* descriptor)
{
    return new WebGPUShaderModule(device, nullptr, descriptor);
}

WebGPUShaderModule::WebGPUShaderModule(WebGPUDevice* device, std::unique_ptr<ShaderModule> shaderModule, WGPUShaderModuleDescriptor const* descriptor)
    : m_wgpuDevice(device)
    , m_descriptor(*descriptor)
    , m_shaderModule(std::move(shaderModule))
{
}

ShaderModule* WebGPUShaderModule::getShaderModule() const
{
    return m_shaderModule.get();
}

} // namespace jipu