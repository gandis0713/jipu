#include "webgpu_adapter.h"

#include "webgpu_device.h"
#include "webgpu_instance.h"

namespace jipu
{

WebGPUAdapter::WebGPUAdapter(WebGPUInstance* instance, WGPU_NULLABLE WGPURequestAdapterOptions const* options)
    : m_wgpuInstance(instance)
{
}

void WebGPUAdapter::requestDevice(WGPUDeviceDescriptor const* descriptor, WGPURequestDeviceCallback callback, void* userdata)
{
    // TODO: Correct the device creation logic.
    auto device = new WebGPUDevice(this, descriptor);
    callback(WGPURequestDeviceStatus::WGPURequestDeviceStatus_Success, reinterpret_cast<WGPUDevice>(device), "", userdata);
}

} // namespace jipu