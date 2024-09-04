#include "webgpu.h"

#include "webgpu/webgpu_adapter.h"
#include "webgpu/webgpu_instance.h"

#include <unordered_map>

namespace jipu
{

WGPUInstance procCreateInstance(WGPUInstanceDescriptor const* wgpuDescriptor)
{
    return reinterpret_cast<WGPUInstance>(WebGPUInstance::create(wgpuDescriptor));
}

void procInstanceRequestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const* options, WGPURequestAdapterCallback callback, void* userdata)
{
    WebGPUInstance* webgpuInstance = reinterpret_cast<WebGPUInstance*>(instance);
    webgpuInstance->requestAdapter(options, callback, userdata);
}

WGPUSurface procInstanceCreateSurface(WGPUInstance instance, WGPUSurfaceDescriptor const* descriptor)
{
    WebGPUInstance* webgpuInstance = reinterpret_cast<WebGPUInstance*>(instance);
    return reinterpret_cast<WGPUSurface>(webgpuInstance->createSurface(descriptor));
}

void procAdapterRequestDevice(WGPUAdapter adapter, WGPU_NULLABLE WGPUDeviceDescriptor const* descriptor, WGPURequestDeviceCallback callback, void* userdata)
{
    WebGPUAdapter* webgpuAdapter = reinterpret_cast<WebGPUAdapter*>(adapter);
    webgpuAdapter->requestDevice(descriptor, callback, userdata);
}

namespace
{
std::unordered_map<const char*, WGPUProc> sProcMap{
    { "wgpuCreateInstance", reinterpret_cast<WGPUProc>(procCreateInstance) },
};

} // namespace

WGPUProc procGetProcAddress(WGPUDevice, char const* procName)
{
    if (procName == nullptr)
    {
        return nullptr;
    }

    if (sProcMap.contains(procName))
    {
        return sProcMap[procName];
    }

    return nullptr;
}

} // namespace jipu