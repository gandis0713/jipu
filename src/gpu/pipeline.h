#pragma once

namespace vkt
{

class Device;
class RenderPass;

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

    void setRenderPass(RenderPass* renderPass);

protected:
    Device* m_device{ nullptr };
    RenderPass* m_renderPass{ nullptr };
};

} // namespace vkt
