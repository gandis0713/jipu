#pragma once

#include "export.h"

namespace vkt
{

struct SurfaceDescriptor
{
    void* windowHandle;
};

class Driver;
class VKT_EXPORT Surface
{
public:
    Surface() = delete;
    Surface(Driver* driver, SurfaceDescriptor descriptor);
    virtual ~Surface() = default;

    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;

protected:
    Driver* m_driver = nullptr;

protected:
    void* m_windowHandle = nullptr;
};

}; // namespace vkt
