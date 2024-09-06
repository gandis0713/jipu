#include "webgpu_texture_view.h"

namespace jipu
{

WebGPUTextureView* create(WebGPUDevice* device, WGPUTextureViewDescriptor const* descriptor)
{
    return new WebGPUTextureView(device, nullptr, descriptor);
}

WebGPUTextureView::WebGPUTextureView(WebGPUDevice* device, std::unique_ptr<TextureView> textureView, WGPUTextureViewDescriptor const* descriptor)
    : m_wgpuDevice(device)
    , m_descriptor(*descriptor)
    , m_textureView(std::move(textureView))
{
}

TextureView* WebGPUTextureView::getTextureView() const
{
    return m_textureView.get();
}

} // namespace jipu