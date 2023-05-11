#pragma once

#include "utils/cast.h"
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

DOWN_CAST(VulkanPipelineLayout, PipelineLayout);

} // namespace vkt