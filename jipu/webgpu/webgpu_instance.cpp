#include "webgpu_instance.h"

#include "webgpu_adapter.h"
#include "webgpu_surface.h"

namespace jipu
{

WebGPUInstance::WebGPUInstance(WGPUInstanceDescriptor const* descriptor)
{
}

void WebGPUInstance::requestAdapter(WGPURequestAdapterOptions const* options, WGPURequestAdapterCallback callback, void* userdata)
{
    // TODO: Correct the adapter creation logic.
    auto adapter = new WebGPUAdapter(this, options);
    callback(WGPURequestAdapterStatus::WGPURequestAdapterStatus_Success, reinterpret_cast<WGPUAdapter>(adapter), "", userdata);
}

WebGPUSurface* WebGPUInstance::createSurface(WGPUSurfaceDescriptor const* descriptor)
{
    return new WebGPUSurface(this, descriptor);
}

} // namespace jipu