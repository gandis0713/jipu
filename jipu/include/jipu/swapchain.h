#pragma once

#include "export.h"
#include "jipu/texture.h"
#include "jipu/texture_view.h"

#include <memory>
#include <vector>

namespace jipu
{

class Device;
class Surface;
class Queue;

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

struct SwapchainDescriptor
{
    TextureFormat textureFormat = TextureFormat::kUndefined;
    PresentMode presentMode = PresentMode::kUndefined;
    ColorSpace colorSpace = ColorSpace::kUndefined;
    uint32_t width = 0;
    uint32_t height = 0;
    Surface* surface = nullptr;
};

class JIPU_EXPORT Swapchain
{
public:
    Swapchain() = delete;
    Swapchain(Device* device, const SwapchainDescriptor& descriptor) noexcept;
    virtual ~Swapchain() noexcept = default;

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;

    virtual void present(Queue* queue) = 0;
    virtual int acquireNextTexture() = 0;
    virtual TextureView* getTextureView(uint32_t index) = 0;

    std::vector<Texture*> getTextures() const;
    std::vector<TextureView*> getTextureViews() const;

    TextureFormat getTextureFormat() const;
    PresentMode getPresentMode() const;

    uint32_t getWidth() const;
    uint32_t getHeight() const;

protected:
    Device* m_device = nullptr;
    Surface* m_surface = nullptr;

    std::vector<std::unique_ptr<Texture>> m_textures{};
    std::vector<std::unique_ptr<TextureView>> m_textureViews{};

    TextureFormat m_textureFormat{ TextureFormat::kUndefined };
    PresentMode m_presentMode{ PresentMode::kUndefined };
    ColorSpace m_colorSpace{ ColorSpace::kUndefined };
    uint32_t m_width{ 0 };
    uint32_t m_height{ 0 };
};

} // namespace jipu
