#pragma once

#include "export.h"

#include "jipu/texture.h"

namespace jipu
{

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

enum class CompositeAlphaFlag
{
    kUndefined = 0x00000000,
    kOpaque = 0x00000001,
    kPreMultiplied = 0x00000002,
    kPostMultiplied = 0x00000003,
    kInherit = 0x00000004,
};

struct SurfaceCapabilities
{
    std::vector<TextureFormat> formats{};
    std::vector<PresentMode> presentModes{};
    std::vector<CompositeAlphaFlag> compositeAlphaFlags{};
};

struct SurfaceDescriptor
{
    void* windowHandle;
};

class JIPU_EXPORT Surface
{
public:
    virtual ~Surface() = default;

    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;

protected:
    Surface() = default;
};

}; // namespace jipu
