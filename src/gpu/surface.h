#pragma once

namespace vkt
{

class Platform;

struct SurfaceDescriptor
{
};

class Surface
{
public:
    Surface() = delete;
    Surface(Platform* platform, SurfaceDescriptor descriptor);
    virtual ~Surface() = default;

    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;

protected:
    Platform* m_platform;
};

}; // namespace vkt
