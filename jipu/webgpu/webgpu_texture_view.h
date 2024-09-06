
#pragma once

#include "common/ref_counted.h"
#include "jipu/texture_view.h"
#include "webgpu_header.h"

#include <memory>

namespace jipu
{

class WebGPUDevice;
class WebGPUTextureView : public RefCounted
{

public:
    static WebGPUTextureView* create(WebGPUDevice* device, WGPUTextureViewDescriptor const* descriptor);

public:
    WebGPUTextureView() = delete;
    explicit WebGPUTextureView(WebGPUDevice* device, std::unique_ptr<TextureView> textureView, WGPUTextureViewDescriptor const* descriptor);

public:
    virtual ~WebGPUTextureView() = default;

    WebGPUTextureView(const WebGPUTextureView&) = delete;
    WebGPUTextureView& operator=(const WebGPUTextureView&) = delete;

public: // WebGPU API
public:
    TextureView* getTextureView() const;

private:
    [[maybe_unused]] WebGPUDevice* m_wgpuDevice = nullptr;
    [[maybe_unused]] const WGPUTextureViewDescriptor m_descriptor{};

private:
    std::unique_ptr<TextureView> m_textureView = nullptr;
};

} // namespace jipu