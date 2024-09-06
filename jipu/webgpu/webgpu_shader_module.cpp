#include "webgpu_shader_module.h"

#include "webgpu_device.h"

namespace jipu
{

WebGPUShaderModule* WebGPUShaderModule::create(WebGPUDevice* wgpuDevice, WGPUShaderModuleDescriptor const* descriptor)
{
    ShaderModuleDescriptor shaderModuleDescriptor{};

    const WGPUChainedStruct* current = descriptor->nextInChain;
    while (current)
    {
        switch (current->sType)
        {
        case WGPUSType_ShaderModuleWGSLDescriptor: {
            WGPUShaderModuleWGSLDescriptor const* wgslDescriptor = reinterpret_cast<WGPUShaderModuleWGSLDescriptor const*>(current);
            shaderModuleDescriptor.code = wgslDescriptor->code;
            shaderModuleDescriptor.codeSize = strlen(wgslDescriptor->code);
        }
        break;
        case WGPUSType_ShaderModuleSPIRVDescriptor: {
            WGPUShaderModuleSPIRVDescriptor const* spirvDescriptor = reinterpret_cast<WGPUShaderModuleSPIRVDescriptor const*>(current);
            shaderModuleDescriptor.code = reinterpret_cast<const char*>(spirvDescriptor->code);
            shaderModuleDescriptor.codeSize = spirvDescriptor->codeSize;
        }
        break;
        default:
            throw std::runtime_error("Unsupported WGPUShaderModuleDescriptor type");
        }

        current = current->next;
    }

    auto device = wgpuDevice->getDevice();
    auto shaderModule = device->createShaderModule(shaderModuleDescriptor);

    return new WebGPUShaderModule(wgpuDevice, std::move(shaderModule), descriptor);
}

WebGPUShaderModule::WebGPUShaderModule(WebGPUDevice* wgpuDevice, std::unique_ptr<ShaderModule> shaderModule, WGPUShaderModuleDescriptor const* descriptor)
    : m_wgpuDevice(wgpuDevice)
    , m_descriptor(*descriptor)
    , m_shaderModule(std::move(shaderModule))
{
}

ShaderModule* WebGPUShaderModule::getShaderModule() const
{
    return m_shaderModule.get();
}

} // namespace jipu