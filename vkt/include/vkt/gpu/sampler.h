#pragma once

namespace vkt
{

enum class AddressMode
{
    kClampToEdge = 0,
    kRepeat,
    kMirrorRepeat
};

enum class FilterMode
{
    kNearest = 0,
    kLinear,
};

enum class MipmapFilterMode
{
    kNearest = 0,
    kLinear,
};

struct SamplerDescriptor
{
    AddressMode addressModeU = AddressMode::kClampToEdge;
    AddressMode addressModeV = AddressMode::kClampToEdge;
    AddressMode addressModeW = AddressMode::kClampToEdge;
    FilterMode magFilter = FilterMode::kNearest;
    FilterMode minFilter = FilterMode::kNearest;
    MipmapFilterMode mipmapFilter = MipmapFilterMode::kNearest;
    float lodMin = 0;
    float lodMax = 32;
};

class Device;
class Sampler
{
public:
    Sampler() = delete;
    Sampler(Device* device, const SamplerDescriptor& descriptor);
    virtual ~Sampler() = default;

protected:
    Device* m_device = nullptr;
};

} // namespace vkt