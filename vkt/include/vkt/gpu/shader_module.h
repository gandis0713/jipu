#pragma once

#include <cstddef>

namespace vkt
{

struct ShaderModuleDescriptor
{
    const char* code = nullptr;
    size_t codeSize = 0;
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