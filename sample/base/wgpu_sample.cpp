#include "wgpu_sample.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

WGPUSample::WGPUSample(const WGPUSampleDescriptor& descriptor)
    : Window(descriptor.windowDescriptor)
    , m_appPath(descriptor.path)
    , m_appDir(descriptor.path.parent_path())
{
}

WGPUSample::~WGPUSample()
{
    // TODO: check ways release and destory.

    wgpuQueueRelease(m_wgpuContext.queue);
    wgpuDeviceDestroy(m_wgpuContext.device);
    wgpuDeviceRelease(m_wgpuContext.device);
    wgpuAdapterRelease(m_wgpuContext.adapter);
    wgpuInstanceRelease(m_wgpuContext.instance);
}

void WGPUSample::createInstance()
{
    WGPUInstanceDescriptor descriptor{};
    descriptor.nextInChain = NULL;
    m_wgpuContext.instance = wgpuCreateInstance({}); // TODO

    assert(m_wgpuContext.instance);
}

void WGPUSample::createSurface()
{
    WGPUChainedStruct chain{};
    chain.sType = WGPUSType_SurfaceDescriptorFromMetalLayer;

    WGPUSurfaceDescriptorFromMetalLayer metalLayerDesc = {};
    metalLayerDesc.chain = chain;
    metalLayerDesc.layer = getWindowHandle();

    WGPUSurfaceDescriptor surfaceDesc = {};
    surfaceDesc.nextInChain = reinterpret_cast<WGPUChainedStruct const*>(&metalLayerDesc);
    surfaceDesc.label = "Metal Surface";

    m_wgpuContext.surface = wgpuInstanceCreateSurface(m_wgpuContext.instance, &surfaceDesc);

    assert(m_wgpuContext.surface);
}

void WGPUSample::createAdapter()
{
    auto cb = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, WGPU_NULLABLE void* userdata) {
        if (status != WGPURequestAdapterStatus_Success)
        {
            throw std::runtime_error("Failed to request adapter.");
        }

        *static_cast<WGPUAdapter*>(userdata) = adapter;
    };

    WGPURequestAdapterOptions descriptor{
        .backendType = WGPUBackendType_Metal,
        .compatibleSurface = m_wgpuContext.surface,
        .forceFallbackAdapter = false,
        .powerPreference = WGPUPowerPreference_HighPerformance
    };
    wgpuInstanceRequestAdapter(m_wgpuContext.instance, &descriptor, cb, &m_wgpuContext.adapter);

    assert(m_wgpuContext.adapter);
}

void WGPUSample::createDevice()
{
    auto cb = [](WGPURequestDeviceStatus status, WGPUDevice device, char const* message, WGPU_NULLABLE void* userdata) {
        if (status != WGPURequestDeviceStatus_Success)
        {
            throw std::runtime_error("Failed to request device.");
        }

        *static_cast<WGPUDevice*>(userdata) = device;
    };

    WGPUDeviceDescriptor deviceDescriptor{};
    wgpuAdapterRequestDevice(m_wgpuContext.adapter, &deviceDescriptor, cb, &m_wgpuContext.device);

    assert(m_wgpuContext.device);
}

void WGPUSample::createSurfaceConfigure()
{
    wgpuSurfaceGetCapabilities(m_wgpuContext.surface, m_wgpuContext.adapter, &m_wgpuContext.surfaceCapabilities);

    m_wgpuContext.surfaceConfigure = {
        .device = m_wgpuContext.device,
        .usage = WGPUTextureUsage_RenderAttachment,
        .format = m_wgpuContext.surfaceCapabilities.formats[0],
        .presentMode = WGPUPresentMode_Fifo,
        .alphaMode = m_wgpuContext.surfaceCapabilities.alphaModes[0],
        .width = m_width,
        .height = m_height,
    };

    wgpuSurfaceConfigure(m_wgpuContext.surface, &m_wgpuContext.surfaceConfigure);
}

void WGPUSample::createQueue()
{
    m_wgpuContext.queue = wgpuDeviceGetQueue(m_wgpuContext.device);

    assert(m_wgpuContext.queue);
}

void WGPUSample::init()
{
    createInstance();
    createSurface();
    createAdapter();
    createDevice();
    createSurfaceConfigure();
    createQueue();
}

void WGPUSample::update()
{
}

} // namespace jipu