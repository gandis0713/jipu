
#pragma once

#include <webgpu.h>

#include "common/ref_counted.h"

#include "jipu/binding_group.h"

namespace jipu
{

class WebGPUDevice;
class WebGPUBindGroup : public RefCounted
{

public:
    static WebGPUBindGroup* create(WebGPUDevice* device, WGPUBindGroupDescriptor const* descriptor);

public:
    WebGPUBindGroup() = delete;
    explicit WebGPUBindGroup(WebGPUDevice* device, std::unique_ptr<BindingGroup> layout, WGPUBindGroupDescriptor const* descriptor);

public:
    virtual ~WebGPUBindGroup() = default;

    WebGPUBindGroup(const WebGPUBindGroup&) = delete;
    WebGPUBindGroup& operator=(const WebGPUBindGroup&) = delete;

public: // WebGPU API
public:
    BindingGroup* getBindingGroup() const;

private:
    [[maybe_unused]] WebGPUDevice* m_wgpuDevice = nullptr;
    [[maybe_unused]] const WGPUBindGroupDescriptor m_descriptor{};

private:
    std::unique_ptr<BindingGroup> m_layout = nullptr;
};

} // namespace jipu