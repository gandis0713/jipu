#pragma once

#include <webgpu.h>

#include "common/ref_counted.h"

#include "jipu/device.h"
#include "jipu/instance.h"
#include "jipu/physical_device.h"

namespace jipu
{

class WebGPUInstance;
class WebGPUAdapter : public RefCounted
{
public:
    WebGPUAdapter() = delete;
    explicit WebGPUAdapter(WebGPUInstance* instance, WGPU_NULLABLE WGPURequestAdapterOptions const* options);

public:
    virtual ~WebGPUAdapter() = default;

    WebGPUAdapter(const WebGPUAdapter&) = delete;
    WebGPUAdapter& operator=(const WebGPUAdapter&) = delete;

public:
    void requestDevice(WGPUDeviceDescriptor const* descriptor, WGPURequestDeviceCallback callback, void* userdata);

private:
    [[maybe_unused]] WebGPUInstance* m_wgpuInstance = nullptr;

private:
    std::unique_ptr<Instance> m_instance = nullptr;
};

} // namespace jipu