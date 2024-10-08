#pragma once

#include "vulkan_api.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace jipu
{

struct InflightResource
{
    std::unordered_set<VkCommandBuffer> commandBuffers{};
    std::unordered_set<VkBuffer> buffers{};
    std::unordered_set<VkImage> images{};
    std::unordered_set<VkImageView> imageViews{};
    std::unordered_set<VkSemaphore> signalSemaphores{};
    // std::unordered_set<VkSemaphore> waitSemaphores{};
    std::unordered_set<VkSampler> samplers{};
    std::unordered_set<VkPipeline> pipelines{};
    std::unordered_set<VkPipelineLayout> pipelineLayouts{};
    std::unordered_set<VkDescriptorSet> descriptorSet{};
    std::unordered_set<VkDescriptorSetLayout> descriptorSetLayouts{};
    std::unordered_set<VkFramebuffer> framebuffers{};
    std::unordered_set<VkRenderPass> renderPasses{};
};

class VulkanDevice;
class CommandBuffer;
class VulkanInflightResource final
{
public:
    static InflightResource generate(std::vector<CommandBuffer*> commandBuffers);

public:
    VulkanInflightResource() = delete;
    explicit VulkanInflightResource(VulkanDevice* device);
    ~VulkanInflightResource();

private:
    [[maybe_unused]] VulkanDevice* m_device = nullptr;

private:
    std::unordered_map<VkFence, InflightResource> m_fences{};
};

} // namespace jipu