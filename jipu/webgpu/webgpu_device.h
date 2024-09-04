#pragma once

#include <webgpu.h>

#include "common/ref_counted.h"

#include "jipu/device.h"

namespace jipu
{

class WebGPUAdapter;
class WebGPUQueue;
class WebGPUDevice : public RefCounted
{
public:
    static WebGPUDevice* create(WebGPUAdapter* wgpuAdapter, WGPUDeviceDescriptor const* wgpuDescriptor);

public:
    WebGPUDevice() = delete;
    explicit WebGPUDevice(WebGPUAdapter* wgpuAdapter, std::unique_ptr<Device> device);
    explicit WebGPUDevice(WebGPUAdapter* wgpuAdapter, std::unique_ptr<Device> device, WGPUDeviceDescriptor const* wgpuDescriptor);

public:
    virtual ~WebGPUDevice() = default;

    WebGPUDevice(const WebGPUDevice&) = delete;
    WebGPUDevice& operator=(const WebGPUDevice&) = delete;

public: // WebGPU API
    WebGPUQueue* getQueue();

public:
    Device* getDevice() const;

private:
    [[maybe_unused]] WebGPUAdapter* m_wgpuAdapter = nullptr;
    [[maybe_unused]] const WGPUDeviceDescriptor m_descriptor{};

private:
    std::unique_ptr<Device> m_device = nullptr;
};

} // namespace jipu