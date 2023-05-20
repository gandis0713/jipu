#pragma once

namespace vkt
{

struct ShaderModuleDescriptor
{
};

class Device;
class ShaderModule
{
public:
    ShaderModule() = delete;
    ShaderModule(Device* device, const ShaderModuleDescriptor& descriptor);
    virtual ~ShaderModule() = default;

protected:
    Device* m_device = nullptr;
};

} // namespace vkt