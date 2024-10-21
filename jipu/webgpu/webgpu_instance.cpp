#include "webgpu_instance.h"

#include "webgpu_adapter.h"
#include "webgpu_surface.h"

namespace jipu
{

WebGPUInstance* WebGPUInstance::create(WGPUInstanceDescriptor const* wgpuDescriptor)
{
    if (wgpuDescriptor)
    {
        return new WebGPUInstance(wgpuDescriptor);
    }

    return new WebGPUInstance();
}

WebGPUInstance::WebGPUInstance()
    : m_wgpuDescriptor()
{
}

WebGPUInstance::WebGPUInstance(WGPUInstanceDescriptor const* wgpuDescriptor)
    : m_wgpuDescriptor(*wgpuDescriptor)
{
}

void WebGPUInstance::requestAdapter(WGPURequestAdapterOptions const* options, WGPURequestAdapterCallback callback, void* userdata)
{
    auto adapter = WebGPUAdapter::create(this, options);
    if (adapter)
    {
        callback(WGPURequestAdapterStatus::WGPURequestAdapterStatus_Success, reinterpret_cast<WGPUAdapter>(adapter), "Succeed to create adapter", userdata);
    }
    else
    {
        callback(WGPURequestAdapterStatus::WGPURequestAdapterStatus_Error, nullptr, "Failed to create adapter", userdata);
    }
}

WebGPUSurface* WebGPUInstance::createSurface(WGPUSurfaceDescriptor const* descriptor)
{
    return new WebGPUSurface(this, descriptor);
}

} // namespace jipu