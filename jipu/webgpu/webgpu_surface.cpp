#include "webgpu_surface.h"

#include "webgpu_adapter.h"
#include "webgpu_instance.h"

namespace jipu
{

WebGPUSurface::WebGPUSurface(WebGPUInstance* instance, WGPUSurfaceDescriptor const* descriptor)
    : m_wgpuInstance(instance)
    , m_wgpuAdapter(nullptr)
{
}

} // namespace jipu