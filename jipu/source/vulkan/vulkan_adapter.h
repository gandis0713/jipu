#pragma once

#include "jipu/adapter.h"

#include "vulkan_instance.h"

namespace jipu
{

class VulkanAdapter : public Adapter
{

public:
    explicit VulkanAdapter(VulkanInstance& instance, const AdapterDescriptor& descriptor);

    VulkanAdapter(const Adapter&) = delete;
    VulkanAdapter& operator=(const Adapter&) = delete;

public: // WebGPU API
    AdapterInfo wgpuGetInfo() override;
    void wgpuRequestDevice(const DeviceDescriptor& descriptor, AdapterRequestDeviceCallback callback, void* userdata) override;

private:
    VulkanInstance& m_instance;
    const AdapterDescriptor m_descriptor;
};

} // namespace jipu