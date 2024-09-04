#include "webgpu_surface.h"

#include "webgpu_adapter.h"
#include "webgpu_device.h"
#include "webgpu_instance.h"

namespace jipu
{

WebGPUSurface* WebGPUSurface::create(WebGPUInstance* instance, WGPUSurfaceDescriptor const* descriptor)
{
    return new WebGPUSurface(instance, descriptor);
}

WebGPUSurface::WebGPUSurface(WebGPUInstance* instance, WGPUSurfaceDescriptor const* descriptor)
    : m_wgpuInstance(instance)
    , m_wgpuDevice(nullptr)
    , m_descriptor(*descriptor)
    , m_surface(nullptr)
    , m_swapchain(nullptr)
{
}

WGPUStatus WebGPUSurface::getCapabilities(WebGPUAdapter* adapter, WGPUSurfaceCapabilities* capabilities)
{
    auto physicalDevice = adapter->getPhysicalDevice();

    if (m_surface == nullptr)
    {
        // auto instance = physicalDevice->getInstance();
        // m_surface = instance->createSurface(SurfaceDescriptor{ .windowHandle = nullptr });
    }

    auto surfaceCapabilities = physicalDevice->getSurfaceCapabilities(m_surface.get());

    auto formatSize = surfaceCapabilities.formats.size();
    auto formatArray = new WGPUTextureFormat[formatSize];
    for (auto i = 0; i < formatSize; ++i)
    {
        formatArray[i] = ToWGPUTextureFormat(surfaceCapabilities.formats[i]);
    }
    capabilities->formats = formatArray;
    capabilities->formatCount = formatSize;

    auto presentModeSize = surfaceCapabilities.presentModes.size();
    auto presentModeArray = new WGPUPresentMode[presentModeSize];
    for (auto i = 0; i < presentModeSize; ++i)
    {
        presentModeArray[i] = static_cast<WGPUPresentMode>(surfaceCapabilities.presentModes[i]);
    }
    capabilities->presentModes = presentModeArray;
    capabilities->presentModeCount = presentModeSize;

    auto compositeAlphaSize = surfaceCapabilities.compositeAlphaFlags.size();
    auto compositeAlphaArray = new WGPUCompositeAlphaMode[compositeAlphaSize];
    for (auto i = 0; i < compositeAlphaSize; ++i)
    {
        compositeAlphaArray[i] = ToWGPUCompositeAlphaMode(surfaceCapabilities.compositeAlphaFlags[i]);
    }
    capabilities->alphaModes = compositeAlphaArray;
    capabilities->alphaModeCount = compositeAlphaSize;

    return WGPUStatus::WGPUStatus_Success;
}

// Convert from WebGPU to JIPU
WGPUTextureFormat ToWGPUTextureFormat(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::kBGRA_8888_UInt_Norm:
        return WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm;
    case TextureFormat::kBGRA_8888_UInt_Norm_SRGB:
        return WGPUTextureFormat::WGPUTextureFormat_BGRA8UnormSrgb;
    // case TextureFormat::kRGB_888_UInt_Norm:
    // case TextureFormat::kRGB_888_UInt_Norm_SRGB:
    case TextureFormat::kRGBA_8888_UInt_Norm:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;
    case TextureFormat::kRGBA_8888_UInt_Norm_SRGB:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA8UnormSrgb;
    case TextureFormat::kRGBA_16161616_UInt_Norm:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA16Unorm;
    case TextureFormat::kD_32_SFloat:
        return WGPUTextureFormat::WGPUTextureFormat_Depth32Float;
    case TextureFormat::kD_24_UInt_Norm_S_8_UInt:
        return WGPUTextureFormat::WGPUTextureFormat_Depth24PlusStencil8;
    default:
        return WGPUTextureFormat::WGPUTextureFormat_Undefined;
    }
}

WGPUCompositeAlphaMode ToWGPUCompositeAlphaMode(CompositeAlphaFlag flag)
{
    switch (flag)
    {
    case CompositeAlphaFlag::kUndefined:
    default:
        return WGPUCompositeAlphaMode_Auto;
    case CompositeAlphaFlag::kOpaque:
        return WGPUCompositeAlphaMode::WGPUCompositeAlphaMode_Opaque;
    case CompositeAlphaFlag::kPreMultiplied:
        return WGPUCompositeAlphaMode::WGPUCompositeAlphaMode_Premultiplied;
    case CompositeAlphaFlag::kPostMultiplied:
        return WGPUCompositeAlphaMode::WGPUCompositeAlphaMode_Unpremultiplied;
    case CompositeAlphaFlag::kInherit:
        return WGPUCompositeAlphaMode::WGPUCompositeAlphaMode_Inherit;
    }
}

} // namespace jipu