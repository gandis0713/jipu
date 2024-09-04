#pragma once

#include <webgpu.h>

#include "common/ref_counted.h"

#include "jipu/device.h"

namespace jipu
{

class WebGPUAdapter;
class WebGPUDevice : public RefCounted
{
public:
    WebGPUDevice() = delete;
    explicit WebGPUDevice(WebGPUAdapter* adapter, WGPUDeviceDescriptor const* descriptor);

public:
    virtual ~WebGPUDevice() = default;

    WebGPUDevice(const WebGPUDevice&) = delete;
    WebGPUDevice& operator=(const WebGPUDevice&) = delete;

private:
    [[maybe_unused]] WebGPUAdapter* m_wgpuAdapter = nullptr;

private:
    std::unique_ptr<Device> m_device = nullptr;
};

} // namespace jipu