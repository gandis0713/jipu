
#pragma once

#include <webgpu.h>

#include "common/ref_counted.h"

#include "jipu/texture.h"

namespace jipu
{

class WebGPUDevice;
class WebGPUTexture : public RefCounted
{

public:
    static WebGPUTexture* create(WebGPUDevice* device, WGPUTextureDescriptor const* descriptor);

public:
    WebGPUTexture() = delete;
    explicit WebGPUTexture(WebGPUDevice* device, std::unique_ptr<Texture> texture, WGPUTextureDescriptor const* descriptor);

public:
    virtual ~WebGPUTexture() = default;

    WebGPUTexture(const WebGPUTexture&) = delete;
    WebGPUTexture& operator=(const WebGPUTexture&) = delete;

public: // WebGPU API
private:
    [[maybe_unused]] WebGPUDevice* m_wgpuDevice = nullptr;
    [[maybe_unused]] const WGPUTextureDescriptor m_descriptor{};

private:
    std::unique_ptr<Texture> m_texture = nullptr;
};

// Convert from WebGPU to JIPU
WGPUTextureFormat ToWGPUTextureFormat(TextureFormat format);

// Convert from JIPU to WebGPU
TextureFormat ToTextureFormat(WGPUTextureFormat format);

} // namespace jipu