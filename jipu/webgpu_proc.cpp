#include <webgpu.h>

namespace jipu
{

extern WGPUProc procGetProcAddress(WGPUDevice device, char const* procName);
extern WGPUInstance procCreateInstance(WGPUInstanceDescriptor const* wgpuDescriptor);
extern void procInstanceRequestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const* options, WGPURequestAdapterCallback callback, void* userdata);
extern WGPUSurface procInstanceCreateSurface(WGPUInstance instance, WGPUSurfaceDescriptor const* descriptor);

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

    WGPU_EXPORT void wgpuInstanceRequestAdapter(WGPUInstance instance, WGPU_NULLABLE WGPURequestAdapterOptions const* options, WGPURequestAdapterCallback callback, WGPU_NULLABLE void* userdata) WGPU_FUNCTION_ATTRIBUTE
    {
        return procInstanceRequestAdapter(instance, options, callback, userdata);
    }

    WGPU_EXPORT WGPUSurface wgpuInstanceCreateSurface(WGPUInstance instance, WGPUSurfaceDescriptor const* descriptor) WGPU_FUNCTION_ATTRIBUTE
    {
        return procInstanceCreateSurface(instance, descriptor);
    }
}