#pragma once

#include "vkt/gpu/pipeline_layout.h"

namespace vkt
{

class VulkanDevice;
class VulkanPipelineLayout : public PipelineLayout
{
public:
    VulkanPipelineLayout() = delete;
    VulkanPipelineLayout(VulkanDevice* device, const PipelineLayoutDescriptor& descriptor);
    ~VulkanPipelineLayout() override = default;
};

} // namespace vkt