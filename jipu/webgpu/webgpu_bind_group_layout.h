
#pragma once

#include "webgpu_header.h"

#include "common/ref_counted.h"

#include "jipu/binding_group_layout.h"

namespace jipu
{

class WebGPUDevice;
class WebGPUBindGroupLayout : public RefCounted
{

public:
    static WebGPUBindGroupLayout* create(WebGPUDevice* device, WGPUBindGroupLayoutDescriptor const* descriptor);

public:
    WebGPUBindGroupLayout() = delete;
    explicit WebGPUBindGroupLayout(WebGPUDevice* device, std::unique_ptr<BindingGroupLayout> layout, WGPUBindGroupLayoutDescriptor const* descriptor);

public:
    virtual ~WebGPUBindGroupLayout() = default;

    WebGPUBindGroupLayout(const WebGPUBindGroupLayout&) = delete;
    WebGPUBindGroupLayout& operator=(const WebGPUBindGroupLayout&) = delete;

public: // WebGPU API
public:
    BindingGroupLayout* getBindingGroupLayout() const;

private:
    [[maybe_unused]] WebGPUDevice* m_wgpuDevice = nullptr;
    [[maybe_unused]] const WGPUBindGroupLayoutDescriptor m_descriptor{};

private:
    std::unique_ptr<BindingGroupLayout> m_layout = nullptr;
};

} // namespace jipu