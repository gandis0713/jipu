#include "webgpu_texture_view.h"

#include "jipu/texture_view.h"
#include "webgpu_texture.h"

namespace jipu
{

WebGPUTextureView* WebGPUTextureView::create(WebGPUTexture* wgpuTexture, WGPUTextureViewDescriptor const* descriptor)
{
    TextureViewDescriptor viewDescriptor{};
    viewDescriptor.dimension = ToTextureViewDimension(descriptor->dimension);
    viewDescriptor.aspect = ToTextureAspectFlags(descriptor->aspect);

    auto textureView = wgpuTexture->getTexture()->createTextureView(viewDescriptor);

    return new WebGPUTextureView(wgpuTexture, std::move(textureView), descriptor);
}

WebGPUTextureView::WebGPUTextureView(WebGPUTexture* wgpuTexture, std::unique_ptr<TextureView> textureView, WGPUTextureViewDescriptor const* descriptor)
    : m_wgpuTexture(wgpuTexture)
    , m_descriptor(*descriptor)
    , m_textureView(std::move(textureView))
{
}

TextureView* WebGPUTextureView::getTextureView() const
{
    return m_textureView.get();
}

// Convert from WebGPU to JIPU
WGPUTextureViewDimension ToWGPUTextureViewDimension(TextureViewDimension dimension)
{
    switch (dimension)
    {
    case TextureViewDimension::kUndefined:
        return WGPUTextureViewDimension::WGPUTextureViewDimension_Undefined;
    case TextureViewDimension::k1D:
        return WGPUTextureViewDimension::WGPUTextureViewDimension_1D;
    case TextureViewDimension::k2D:
        return WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
    case TextureViewDimension::k2DArray:
        return WGPUTextureViewDimension::WGPUTextureViewDimension_2DArray;
    case TextureViewDimension::kCube:
        return WGPUTextureViewDimension::WGPUTextureViewDimension_Cube;
    case TextureViewDimension::kCubeArray:
        return WGPUTextureViewDimension::WGPUTextureViewDimension_CubeArray;
    case TextureViewDimension::k3D:
        return WGPUTextureViewDimension::WGPUTextureViewDimension_3D;
    default:
        return WGPUTextureViewDimension::WGPUTextureViewDimension_Undefined;
    }
}

WGPUTextureAspect ToWGPUTextureAspect(TextureAspectFlags aspect)
{
    WGPUTextureAspect wgpuAspect = WGPUTextureAspect::WGPUTextureAspect_Undefined;

    if (aspect & TextureAspectFlagBits::kColor && aspect & TextureAspectFlagBits::kDepth && aspect & TextureAspectFlagBits::kStencil)
    {
        wgpuAspect = WGPUTextureAspect::WGPUTextureAspect_All;
    }
    else if (aspect & TextureAspectFlagBits::kDepth)
    {
        wgpuAspect = WGPUTextureAspect::WGPUTextureAspect_DepthOnly;
    }
    else if (aspect & TextureAspectFlagBits::kStencil)
    {
        wgpuAspect = WGPUTextureAspect::WGPUTextureAspect_StencilOnly;
    }

    return wgpuAspect;
}

// Convert from JIPU to WebGPU
TextureViewDimension ToTextureViewDimension(WGPUTextureViewDimension dimension)
{
    switch (dimension)
    {
    case WGPUTextureViewDimension::WGPUTextureViewDimension_Undefined:
        return TextureViewDimension::kUndefined;
    case WGPUTextureViewDimension::WGPUTextureViewDimension_1D:
        return TextureViewDimension::k1D;
    case WGPUTextureViewDimension::WGPUTextureViewDimension_2D:
        return TextureViewDimension::k2D;
    case WGPUTextureViewDimension::WGPUTextureViewDimension_2DArray:
        return TextureViewDimension::k2DArray;
    case WGPUTextureViewDimension::WGPUTextureViewDimension_Cube:
        return TextureViewDimension::kCube;
    case WGPUTextureViewDimension::WGPUTextureViewDimension_CubeArray:
        return TextureViewDimension::kCubeArray;
    case WGPUTextureViewDimension::WGPUTextureViewDimension_3D:
        return TextureViewDimension::k3D;
    default:
        return TextureViewDimension::kUndefined;
    }
}

TextureAspectFlags ToTextureAspectFlags(WGPUTextureAspect aspect)
{
    TextureAspectFlags flags = TextureAspectFlagBits::kUndefined;

    if (aspect & WGPUTextureAspect::WGPUTextureAspect_All)
    {
        flags |= TextureAspectFlagBits::kColor | TextureAspectFlagBits::kDepth | TextureAspectFlagBits::kStencil;
    }
    else if (aspect & WGPUTextureAspect::WGPUTextureAspect_DepthOnly)
    {
        flags |= TextureAspectFlagBits::kDepth;
    }
    else if (aspect & WGPUTextureAspect::WGPUTextureAspect_StencilOnly)
    {
        flags |= TextureAspectFlagBits::kStencil;
    }

    return flags;
}
} // namespace jipu