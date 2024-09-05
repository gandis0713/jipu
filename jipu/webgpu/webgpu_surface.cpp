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
    const WGPUChainedStruct* current = m_descriptor.nextInChain;
    while (current)
    {
        switch (current->sType)
        {
        case WGPUSType_SurfaceDescriptorFromMetalLayer: {
            m_type = Type::kMetalLayer;
            WGPUSurfaceDescriptorFromMetalLayer const* metalLayer = reinterpret_cast<WGPUSurfaceDescriptorFromMetalLayer const*>(current);
            m_meterLayer = reinterpret_cast<void*>(metalLayer->layer);
        }
        break;
        case WGPUSType_SurfaceDescriptorFromAndroidNativeWindow: {
            m_type = Type::kMetalLayer;
            WGPUSurfaceDescriptorFromAndroidNativeWindow const* androidWindow = reinterpret_cast<WGPUSurfaceDescriptorFromAndroidNativeWindow const*>(current);
            m_androidNativeWindow = reinterpret_cast<void*>(androidWindow->window);
        }
        break;
        case WGPUSType_SurfaceDescriptorFromWindowsHWND: {
            m_type = Type::kMetalLayer;
            WGPUSurfaceDescriptorFromWindowsHWND const* windowHWND = reinterpret_cast<WGPUSurfaceDescriptorFromWindowsHWND const*>(current);
            m_hInstance = reinterpret_cast<void*>(windowHWND->hinstance);
            m_HWND = reinterpret_cast<void*>(windowHWND->hwnd);
        }
        break;
        default:
            // TODO: wayland, Xlib and so on.
            break;
        }

        current = current->next;
    }
}

WGPUStatus WebGPUSurface::getCapabilities(WebGPUAdapter* adapter, WGPUSurfaceCapabilities* capabilities)
{
    auto physicalDevice = adapter->getPhysicalDevice();

    if (m_surface == nullptr)
    {
        auto instance = physicalDevice->getInstance();
        switch (m_type)
        {
        case Type::kMetalLayer:
            m_surface = instance->createSurface(SurfaceDescriptor{ .windowHandle = m_meterLayer });
            break;
        case Type::kWindowsHWND:
            m_surface = instance->createSurface(SurfaceDescriptor{ .windowHandle = m_HWND });
            break;
        case Type::kAndroidWindow:
            m_surface = instance->createSurface(SurfaceDescriptor{ .windowHandle = m_androidNativeWindow });
            break;
        default:
            break;
        }
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
    case TextureFormat::kUndefined:
        return WGPUTextureFormat::WGPUTextureFormat_Undefined;
    case TextureFormat::kR8Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_R8Unorm;
    case TextureFormat::kR8Snorm:
        return WGPUTextureFormat::WGPUTextureFormat_R8Snorm;
    case TextureFormat::kR8Uint:
        return WGPUTextureFormat::WGPUTextureFormat_R8Uint;
    case TextureFormat::kR8Sint:
        return WGPUTextureFormat::WGPUTextureFormat_R8Sint;
    case TextureFormat::kR16Uint:
        return WGPUTextureFormat::WGPUTextureFormat_R16Uint;
    case TextureFormat::kR16Sint:
        return WGPUTextureFormat::WGPUTextureFormat_R16Sint;
    case TextureFormat::kR16Float:
        return WGPUTextureFormat::WGPUTextureFormat_R16Float;
    case TextureFormat::kRG8Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_RG8Unorm;
    case TextureFormat::kRG8Snorm:
        return WGPUTextureFormat::WGPUTextureFormat_RG8Snorm;
    case TextureFormat::kRG8Uint:
        return WGPUTextureFormat::WGPUTextureFormat_RG8Uint;
    case TextureFormat::kRG8Sint:
        return WGPUTextureFormat::WGPUTextureFormat_RG8Sint;
    case TextureFormat::kR32Float:
        return WGPUTextureFormat::WGPUTextureFormat_R32Float;
    case TextureFormat::kR32Uint:
        return WGPUTextureFormat::WGPUTextureFormat_R32Uint;
    case TextureFormat::kR32Sint:
        return WGPUTextureFormat::WGPUTextureFormat_R32Sint;
    case TextureFormat::kRG16Uint:
        return WGPUTextureFormat::WGPUTextureFormat_RG16Uint;
    case TextureFormat::kRG16Sint:
        return WGPUTextureFormat::WGPUTextureFormat_RG16Sint;
    case TextureFormat::kRG16Float:
        return WGPUTextureFormat::WGPUTextureFormat_RG16Float;
    case TextureFormat::kRGBA8Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;
    case TextureFormat::kRGBA8UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA8UnormSrgb;
    case TextureFormat::kRGBA8Snorm:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA8Snorm;
    case TextureFormat::kRGBA8Uint:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA8Uint;
    case TextureFormat::kRGBA8Sint:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA8Sint;
    case TextureFormat::kBGRA8Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm;
    case TextureFormat::kBGRA8UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_BGRA8UnormSrgb;
    case TextureFormat::kRGB10A2Uint:
        return WGPUTextureFormat::WGPUTextureFormat_RGB10A2Uint;
    case TextureFormat::kRGB10A2Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_RGB10A2Unorm;
    case TextureFormat::kRG11B10Ufloat:
        return WGPUTextureFormat::WGPUTextureFormat_RG11B10Ufloat;
    case TextureFormat::kRGB9E5Ufloat:
        return WGPUTextureFormat::WGPUTextureFormat_RGB9E5Ufloat;
    case TextureFormat::kRG32Float:
        return WGPUTextureFormat::WGPUTextureFormat_RG32Float;
    case TextureFormat::kRG32Uint:
        return WGPUTextureFormat::WGPUTextureFormat_RG32Uint;
    case TextureFormat::kRG32Sint:
        return WGPUTextureFormat::WGPUTextureFormat_RG32Sint;
    case TextureFormat::kRGBA16Uint:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA16Uint;
    case TextureFormat::kRGBA16Sint:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA16Sint;
    case TextureFormat::kRGBA16Float:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA16Float;
    case TextureFormat::kRGBA32Float:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA32Float;
    case TextureFormat::kRGBA32Uint:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA32Uint;
    case TextureFormat::kRGBA32Sint:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA32Sint;
    case TextureFormat::kStencil8:
        return WGPUTextureFormat::WGPUTextureFormat_Stencil8;
    case TextureFormat::kDepth16Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_Depth16Unorm;
    case TextureFormat::kDepth24Plus:
        return WGPUTextureFormat::WGPUTextureFormat_Depth24Plus;
    case TextureFormat::kDepth24PlusStencil8:
        return WGPUTextureFormat::WGPUTextureFormat_Depth24PlusStencil8;
    case TextureFormat::kDepth32Float:
        return WGPUTextureFormat::WGPUTextureFormat_Depth32Float;
    case TextureFormat::kBC1RGBAUnorm:
        return WGPUTextureFormat::WGPUTextureFormat_BC1RGBAUnorm;
    case TextureFormat::kBC1RGBAUnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_BC1RGBAUnormSrgb;
    case TextureFormat::kBC2RGBAUnorm:
        return WGPUTextureFormat::WGPUTextureFormat_BC2RGBAUnorm;
    case TextureFormat::kBC2RGBAUnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_BC2RGBAUnormSrgb;
    case TextureFormat::kBC3RGBAUnorm:
        return WGPUTextureFormat::WGPUTextureFormat_BC3RGBAUnorm;
    case TextureFormat::kBC3RGBAUnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_BC3RGBAUnormSrgb;
    case TextureFormat::kBC4RUnorm:
        return WGPUTextureFormat::WGPUTextureFormat_BC4RUnorm;
    case TextureFormat::kBC4RSnorm:
        return WGPUTextureFormat::WGPUTextureFormat_BC4RSnorm;
    case TextureFormat::kBC5RGUnorm:
        return WGPUTextureFormat::WGPUTextureFormat_BC5RGUnorm;
    case TextureFormat::kBC5RGSnorm:
        return WGPUTextureFormat::WGPUTextureFormat_BC5RGSnorm;
    case TextureFormat::kBC6HRGBUfloat:
        return WGPUTextureFormat::WGPUTextureFormat_BC6HRGBUfloat;
    case TextureFormat::kBC6HRGBFloat:
        return WGPUTextureFormat::WGPUTextureFormat_BC6HRGBFloat;
    case TextureFormat::kBC7RGBAUnorm:
        return WGPUTextureFormat::WGPUTextureFormat_BC7RGBAUnorm;
    case TextureFormat::kBC7RGBAUnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_BC7RGBAUnormSrgb;
    case TextureFormat::kETC2RGB8Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ETC2RGB8Unorm;
    case TextureFormat::kETC2RGB8UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ETC2RGB8UnormSrgb;
    case TextureFormat::kETC2RGB8A1Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ETC2RGB8A1Unorm;
    case TextureFormat::kETC2RGB8A1UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ETC2RGB8A1UnormSrgb;
    case TextureFormat::kETC2RGBA8Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ETC2RGBA8Unorm;
    case TextureFormat::kETC2RGBA8UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ETC2RGBA8UnormSrgb;
    case TextureFormat::kEACR11Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_EACR11Unorm;
    case TextureFormat::kEACR11Snorm:
        return WGPUTextureFormat::WGPUTextureFormat_EACR11Snorm;
    case TextureFormat::kEACRG11Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_EACRG11Unorm;
    case TextureFormat::kEACRG11Snorm:
        return WGPUTextureFormat::WGPUTextureFormat_EACRG11Snorm;
    case TextureFormat::kASTC4x4Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC4x4Unorm;
    case TextureFormat::kASTC4x4UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC4x4UnormSrgb;
    case TextureFormat::kASTC5x4Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC5x4Unorm;
    case TextureFormat::kASTC5x4UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC5x4UnormSrgb;
    case TextureFormat::kASTC5x5Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC5x5Unorm;
    case TextureFormat::kASTC5x5UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC5x5UnormSrgb;
    case TextureFormat::kASTC6x5Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC6x5Unorm;
    case TextureFormat::kASTC6x5UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC6x5UnormSrgb;
    case TextureFormat::kASTC6x6Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC6x6Unorm;
    case TextureFormat::kASTC6x6UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC6x6UnormSrgb;
    case TextureFormat::kASTC8x5Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC8x5Unorm;
    case TextureFormat::kASTC8x5UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC8x5UnormSrgb;
    case TextureFormat::kASTC8x6Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC8x6Unorm;
    case TextureFormat::kASTC8x6UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC8x6UnormSrgb;
    case TextureFormat::kASTC8x8Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC8x8Unorm;
    case TextureFormat::kASTC8x8UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC8x8UnormSrgb;
    case TextureFormat::kASTC10x5Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC10x5Unorm;
    case TextureFormat::kASTC10x5UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC10x5UnormSrgb;
    case TextureFormat::kASTC10x6Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC10x6Unorm;
    case TextureFormat::kASTC10x6UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC10x6UnormSrgb;
    case TextureFormat::kASTC10x8Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC10x8Unorm;
    case TextureFormat::kASTC10x8UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC10x8UnormSrgb;
    case TextureFormat::kASTC10x10Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC10x10Unorm;
    case TextureFormat::kASTC10x10UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC10x10UnormSrgb;
    case TextureFormat::kASTC12x10Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC12x10Unorm;
    case TextureFormat::kASTC12x10UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC12x10UnormSrgb;
    case TextureFormat::kASTC12x12Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC12x12Unorm;
    case TextureFormat::kASTC12x12UnormSrgb:
        return WGPUTextureFormat::WGPUTextureFormat_ASTC12x12UnormSrgb;
    case TextureFormat::kR16Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_R16Unorm;
    case TextureFormat::kRG16Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_RG16Unorm;
    case TextureFormat::kRGBA16Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA16Unorm;
    case TextureFormat::kR16Snorm:
        return WGPUTextureFormat::WGPUTextureFormat_R16Snorm;
    case TextureFormat::kRG16Snorm:
        return WGPUTextureFormat::WGPUTextureFormat_RG16Snorm;
    case TextureFormat::kRGBA16Snorm:
        return WGPUTextureFormat::WGPUTextureFormat_RGBA16Snorm;
    case TextureFormat::kR8BG8Biplanar420Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_R8BG8Biplanar420Unorm;
    case TextureFormat::kR10X6BG10X6Biplanar420Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_R10X6BG10X6Biplanar420Unorm;
    case TextureFormat::kR8BG8A8Triplanar420Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_R8BG8A8Triplanar420Unorm;
    case TextureFormat::kR8BG8Biplanar422Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_R8BG8Biplanar422Unorm;
    case TextureFormat::kR8BG8Biplanar444Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_R8BG8Biplanar444Unorm;
    case TextureFormat::kR10X6BG10X6Biplanar422Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_R10X6BG10X6Biplanar422Unorm;
    case TextureFormat::kR10X6BG10X6Biplanar444Unorm:
        return WGPUTextureFormat::WGPUTextureFormat_R10X6BG10X6Biplanar444Unorm;
    case TextureFormat::kExternal:
        return WGPUTextureFormat::WGPUTextureFormat_External;
    case TextureFormat::kForce32:
        return WGPUTextureFormat::WGPUTextureFormat_Force32;
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