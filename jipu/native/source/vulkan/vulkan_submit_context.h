#pragma once

#include <vector>

#include "vulkan_api.h"

#include "vulkan_command_recorder.h"

namespace jipu
{

enum class SubmitType
{
    kNone,
    kRender,
    kCompute,
    kTransfer,
    kPresent,
};

struct VulkanSubmitInfo
{
    SubmitType type = SubmitType::kNone;

    std::vector<VkCommandBuffer> commandBuffers{};
    std::vector<VkSemaphore> signalSemaphores{};
    std::vector<VkSemaphore> waitSemaphores{};
    std::vector<VkPipelineStageFlags> waitStages{};
};

class VulkanSubmitContext final
{
public:
    VulkanSubmitContext() = default;
    ~VulkanSubmitContext() = default;

public:
    static VulkanSubmitContext create(const std::vector<VulkanCommandRecordResult>& results);

public:
    std::vector<VulkanSubmitInfo> getSubmitInfos() const;

private:
    std::vector<VulkanSubmitInfo> m_submitInfos{};
};

} // namespace jipu