#pragma once

namespace vkt
{

class Platform;

struct SurfaceCreateInfo
{
};

class Surface
{
public:
    Surface() = delete;
    Surface(Platform* platform, SurfaceCreateInfo info);
    virtual ~Surface() = default;

    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;

protected:
    Platform* m_platform;
};

}; // namespace vkt
