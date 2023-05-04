#pragma once

#include "export.h"
#include <memory>

namespace vkt
{

struct SurfaceDescriptor
{
    void* windowHandle;
};

class Adapter;
class VKT_EXPORT Surface
{
public:
    Surface() = delete;
    Surface(Adapter* adapter, SurfaceDescriptor descriptor);
    virtual ~Surface() = default;

    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;

protected:
    Adapter* m_adapter{ nullptr };

protected:
    void* m_windowHandle{ nullptr };
};

}; // namespace vkt
