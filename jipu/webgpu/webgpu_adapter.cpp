#include "webgpu_adapter.h"

#include "webgpu_instance.h"

namespace jipu
{

WebGPUAdapter::WebGPUAdapter(WebGPUInstance* instance, WGPU_NULLABLE WGPURequestAdapterOptions const* options)
    : m_wgpuInstance(instance)
{
}

} // namespace jipu