#pragma once

namespace vkt
{

class Device;

struct PipelineDescriptor
{
};

class Pipeline
{
public:
    Pipeline() = delete;
    Pipeline(Device* device, PipelineDescriptor descriptor);
    virtual ~Pipeline() = default;

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

protected:
    Device* m_device{ nullptr };
};

} // namespace vkt
