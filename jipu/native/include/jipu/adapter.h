#pragma once

#include <webgpu.h>

#include "device.h"
#include "physical_device.h"

namespace jipu
{

using AdapterInfo = WGPUAdapterInfo;
struct AdapterDescriptor
{
    PhysicalDevice& physicalDevice;
};

class Adapter
{
public:
    explicit Adapter(const AdapterDescriptor& options);
    virtual ~Adapter() = default;

    Adapter(const Adapter&) = delete;
    Adapter& operator=(const Adapter&) = delete;

protected:
    Adapter() = default;

public: // WebGPU API
    virtual AdapterInfo wgpuGetInfo() = 0;
    virtual void wgpuRequestDevice(const DeviceDescriptor& descriptor, AdapterRequestDeviceCallback callback, void* userdata) = 0;
};

} // namespace jipu