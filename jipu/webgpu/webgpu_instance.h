#pragma once

#include <webgpu.h>

#include "common/ref_counted.h"

namespace jipu
{

class WebGPUSurface;
class WebGPUInstance : public RefCounted
{
public:
    static WebGPUInstance* create(WGPUInstanceDescriptor const* descriptor);

public:
    WebGPUInstance() = delete;
    explicit WebGPUInstance(WGPUInstanceDescriptor const* descriptor);

public:
    virtual ~WebGPUInstance() = default;

    WebGPUInstance(const WebGPUInstance&) = delete;
    WebGPUInstance& operator=(const WebGPUInstance&) = delete;

public:
    void requestAdapter(WGPURequestAdapterOptions const* options, WGPURequestAdapterCallback callback, void* userdata);
    WebGPUSurface* createSurface(WGPUSurfaceDescriptor const* descriptor);
};

} // namespace jipu