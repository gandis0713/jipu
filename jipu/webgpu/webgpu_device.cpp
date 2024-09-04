#include "webgpu_device.h"

#include "webgpu_adapter.h"

namespace jipu
{

WebGPUDevice::WebGPUDevice(WebGPUAdapter* adapter, WGPUDeviceDescriptor const* descriptor)
    : m_wgpuAdapter(adapter)
{
}

} // namespace jipu