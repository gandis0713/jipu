#include "webgpu_header.h"

#include "webgpu/webgpu_adapter.h"
#include "webgpu/webgpu_device.h"
#include "webgpu/webgpu_instance.h"
#include "webgpu/webgpu_surface.h"
#include "webgpu/webgpu_texture.h"

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
    return webgpuInstance->requestAdapter(options, callback, userdata);
}

WGPUSurface procInstanceCreateSurface(WGPUInstance instance, WGPUSurfaceDescriptor const* descriptor)
{
    WebGPUInstance* webgpuInstance = reinterpret_cast<WebGPUInstance*>(instance);
    return reinterpret_cast<WGPUSurface>(webgpuInstance->createSurface(descriptor));
}

void procAdapterRequestDevice(WGPUAdapter adapter, WGPU_NULLABLE WGPUDeviceDescriptor const* descriptor, WGPURequestDeviceCallback callback, void* userdata)
{
    WebGPUAdapter* webgpuAdapter = reinterpret_cast<WebGPUAdapter*>(adapter);
    return webgpuAdapter->requestDevice(descriptor, callback, userdata);
}

WGPUQueue procDeviceGetQueue(WGPUDevice device)
{
    WebGPUDevice* webgpuDevice = reinterpret_cast<WebGPUDevice*>(device);
    return reinterpret_cast<WGPUQueue>(webgpuDevice->getQueue());
}

WGPUStatus procSurfaceGetCapabilities(WGPUSurface surface, WGPUAdapter adapter, WGPUSurfaceCapabilities* capabilities)
{
    WebGPUSurface* webgpuSurface = reinterpret_cast<WebGPUSurface*>(surface);
    WebGPUAdapter* webgpuAdapter = reinterpret_cast<WebGPUAdapter*>(adapter);
    return webgpuSurface->getCapabilities(webgpuAdapter, capabilities);
}

void procSurfaceConfigure(WGPUSurface surface, WGPUSurfaceConfiguration const* config)
{
    WebGPUSurface* webgpuSurface = reinterpret_cast<WebGPUSurface*>(surface);
    return webgpuSurface->configure(config);
}

WGPUBindGroup procDeviceCreateBindGroup(WGPUDevice device, WGPUBindGroupDescriptor const* descriptor)
{
    WebGPUDevice* webgpuDevice = reinterpret_cast<WebGPUDevice*>(device);
    return reinterpret_cast<WGPUBindGroup>(webgpuDevice->createBindGroup(descriptor));
}

WGPUBindGroupLayout procDeviceCreateBindGroupLayout(WGPUDevice device, WGPUBindGroupLayoutDescriptor const* descriptor)
{
    WebGPUDevice* webgpuDevice = reinterpret_cast<WebGPUDevice*>(device);
    return reinterpret_cast<WGPUBindGroupLayout>(webgpuDevice->createBindGroupLayout(descriptor));
}

WGPUPipelineLayout procDeviceCreatePipelineLayout(WGPUDevice device, WGPUPipelineLayoutDescriptor const* descriptor)
{
    WebGPUDevice* webgpuDevice = reinterpret_cast<WebGPUDevice*>(device);
    return reinterpret_cast<WGPUPipelineLayout>(webgpuDevice->createPipelineLayout(descriptor));
}

WGPURenderPipeline procDeviceCreateRenderPipeline(WGPUDevice device, WGPURenderPipelineDescriptor const* descriptor)
{
    WebGPUDevice* webgpuDevice = reinterpret_cast<WebGPUDevice*>(device);
    return reinterpret_cast<WGPURenderPipeline>(webgpuDevice->createRenderPipeline(descriptor));
}

WGPUShaderModule procDeviceCreateShaderModule(WGPUDevice device, WGPUShaderModuleDescriptor const* descriptor)
{
    WebGPUDevice* webgpuDevice = reinterpret_cast<WebGPUDevice*>(device);
    return reinterpret_cast<WGPUShaderModule>(webgpuDevice->createShaderModule(descriptor));
}

void procSurfaceGetCurrentTexture(WGPUSurface surface, WGPUSurfaceTexture* surfaceTexture)
{
    WebGPUSurface* webgpuSurface = reinterpret_cast<WebGPUSurface*>(surface);
    return webgpuSurface->getCurrentTexture(surfaceTexture);
}

WGPUTextureView procTextureCreateView(WGPUTexture texture, WGPU_NULLABLE WGPUTextureViewDescriptor const* descriptor)
{
    WebGPUTexture* webgpuTexture = reinterpret_cast<WebGPUTexture*>(texture);
    return reinterpret_cast<WGPUTextureView>(webgpuTexture->createView(descriptor));
}

namespace
{
std::unordered_map<const char*, WGPUProc> sProcMap{
    { "wgpuCreateInstance", reinterpret_cast<WGPUProc>(procCreateInstance) },
    { "wgpuInstanceRequestAdapter", reinterpret_cast<WGPUProc>(procInstanceRequestAdapter) },
    { "wgpuInstanceCreateSurface", reinterpret_cast<WGPUProc>(procInstanceCreateSurface) },
    { "wgpuAdapterRequestDevice", reinterpret_cast<WGPUProc>(procAdapterRequestDevice) },
    { "wgpuDeviceGetQueue", reinterpret_cast<WGPUProc>(procDeviceGetQueue) },
    { "wgpuSurfaceGetCapabilities", reinterpret_cast<WGPUProc>(procSurfaceGetCapabilities) },
    { "wgpuSurfaceConfigure", reinterpret_cast<WGPUProc>(procSurfaceConfigure) },
    { "wgpuDeviceCreateBindGroup", reinterpret_cast<WGPUProc>(procDeviceCreateBindGroup) },
    { "wgpuDeviceCreateBindGroupLayout", reinterpret_cast<WGPUProc>(procDeviceCreateBindGroupLayout) },
    { "wgpuDeviceCreatePipelineLayout", reinterpret_cast<WGPUProc>(procDeviceCreatePipelineLayout) },
    { "wgpuDeviceCreateRenderPipeline", reinterpret_cast<WGPUProc>(procDeviceCreateRenderPipeline) },
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