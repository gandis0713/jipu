#pragma once

#include "export.h"

namespace jipu
{

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
