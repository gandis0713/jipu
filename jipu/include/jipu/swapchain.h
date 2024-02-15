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
    virtual ~Swapchain() noexcept = default;

    Swapchain(const Swapchain&) = delete;
    Swapchain& operator=(const Swapchain&) = delete;

protected:
    Swapchain() = default;

public:
    virtual TextureFormat getTextureFormat() const = 0;
    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;

    virtual void present(Queue* queue) = 0;
    virtual TextureView* acquireNextTexture() = 0;
};

} // namespace jipu
