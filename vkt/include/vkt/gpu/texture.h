#pragma once

#include "export.h"
#include "vkt/gpu/texture_view.h"
#include <memory>
#include <stdint.h>

namespace jipu
{

struct Extent2D
{
    uint32_t width = 0;
    uint32_t height = 0;
};

struct Extent3D : Extent2D
{
    uint32_t depth = 0;
};

enum class TextureType
{
    kUndefined = 0,
    k1D,
    k2D,
    k3D,
};

enum class TextureFormat
{
    kUndefined = 0,
    kBGRA_8888_UInt_Norm,
    kBGRA_8888_UInt_Norm_SRGB,
    kRGB_888_UInt_Norm,
    kRGB_888_UInt_Norm_SRGB,
    kRGBA_8888_UInt_Norm,
    kRGBA_8888_UInt_Norm_SRGB,
    kRGBA_16161616_UInt_Norm,
    kD_32_SFloat,
    kD_24_UInt_Norm_S_8_UInt,
};

struct TextureUsageFlagBits
{
    static constexpr uint32_t kUndefined = 0x00000000;
    static constexpr uint32_t kCopySrc = 0x00000001;
    static constexpr uint32_t kCopyDst = 0x00000002;
    static constexpr uint32_t kTextureBinding = 0x00000004;
    static constexpr uint32_t kStorageBinding = 0x00000008;
    static constexpr uint32_t kDepthStencil = 0x00000010;
    static constexpr uint32_t kColorAttachment = 0x00000020;
};
using TextureUsageFlags = uint32_t;

struct TextureDescriptor
{
    TextureType type = TextureType::kUndefined;
    TextureFormat format = TextureFormat::kUndefined;
    TextureUsageFlags usage = TextureUsageFlagBits::kUndefined;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t mipLevels = 0;
    uint32_t sampleCount = 0;
};

class Device;
class VKT_EXPORT Texture
{
public:
    Texture() = delete;
    Texture(Device* device, TextureDescriptor descriptor);
    virtual ~Texture() = default;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    virtual std::unique_ptr<TextureView> createTextureView(const TextureViewDescriptor& descriptor) = 0;

public:
    TextureType getType() const;
    TextureFormat getFormat() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getMipLevels() const;
    uint32_t getSampleCount() const;

    Device* getDevice() const;

protected:
    Device* m_device = nullptr;
    TextureDescriptor m_descriptor{};
};

} // namespace jipu
