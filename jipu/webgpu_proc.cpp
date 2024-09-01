#include "webgpu.h"

#include "jipu/instance.h"
#include "jipu/physical_device.h"

namespace jipu
{

extern WGPUProc procGetProcAddress(WGPUDevice device, char const* procName);
extern WGPUInstance procCreateInstance(WGPUInstanceDescriptor const* wgpuDescriptor);
void procInstanceRequestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const* options, WGPUInstanceRequestAdapterCallback callback, void* userdata);
;

} // namespace jipu

extern "C"
{
    using namespace jipu;

    WGPU_EXPORT WGPUProc wgpuGetProcAddress(WGPUDevice device, char const* procName) WGPU_FUNCTION_ATTRIBUTE
    {
        return procGetProcAddress(device, procName);
    }

    WGPU_EXPORT WGPUInstance wgpuCreateInstance(WGPU_NULLABLE WGPUInstanceDescriptor const* descriptor) WGPU_FUNCTION_ATTRIBUTE
    {
        return procCreateInstance(descriptor);
    }

    WGPU_EXPORT void wgpuInstanceRequestAdapter(WGPUInstance instance, WGPU_NULLABLE WGPURequestAdapterOptions const* options, WGPUInstanceRequestAdapterCallback callback, WGPU_NULLABLE void* userdata) WGPU_FUNCTION_ATTRIBUTE
    {
    }
}