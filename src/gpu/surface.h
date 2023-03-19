#pragma once

namespace vkt
{

struct SurfaceCreateInfo
{
};

class Surface
{
public:
    explicit Surface(SurfaceCreateInfo info) noexcept {};
    virtual ~Surface() noexcept = default;

    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;
};

}; // namespace vkt
