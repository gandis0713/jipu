#pragma once

#include "vkt/gpu/texture.h"
#include "vkt/gpu/texture_view.h"

#include <memory>
#include <vector>

namespace vkt
{

class Device;
class Surface;

enum class PresentMode
{
    kUndefined = 0,
    kImmediate,
    kFifo,
    kMailbox,
};

enum class ColorSpace
{
    kUndefined = 0,
    kSRGBNonLinear,
    kSRGBLinear,
};

struct SwapChainDescriptor
{
    TextureFormat textureFormat{ TextureFormat::kUndefined };
    PresentMode presentMode{ PresentMode::kUndefined };
    ColorSpace colorSpace{ ColorSpace::kUndefined };
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    Surface* surface{ nullptr };
};

class SwapChain
{
public:
    SwapChain() = delete;
    SwapChain(Device* device, const SwapChainDescriptor& descriptor) noexcept;
    virtual ~SwapChain() noexcept = default;

    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;

    std::vector<Texture*> getTextures() const;
    std::vector<TextureView*> getTextureViews() const;

    TextureFormat getTextureFormat() const;
    PresentMode getPresentMode() const;

    uint32_t getWidth() const;
    uint32_t getHeight() const;

protected:
    Device* m_device{ nullptr };
    Surface* m_surface{ nullptr };

    std::vector<std::unique_ptr<Texture>> m_textures{};
    std::vector<std::unique_ptr<TextureView>> m_textureViews{};

    TextureFormat m_textureFormat{ TextureFormat::kUndefined };
    PresentMode m_presentMode{ PresentMode::kUndefined };
    ColorSpace m_colorSpace{ ColorSpace::kUndefined };
    uint32_t m_width{ 0 };
    uint32_t m_height{ 0 };
};

} // namespace vkt
