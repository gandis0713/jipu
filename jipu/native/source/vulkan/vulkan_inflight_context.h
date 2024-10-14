#pragma once

#include "vulkan_api.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace jipu
{

struct VulkanInflight
{
    struct Object
    {
        std::unordered_set<VkCommandBuffer> commandBuffers{};
        std::unordered_set<VkBuffer> buffers{};
        std::unordered_set<VkImage> images{};
        std::unordered_set<VkImageView> imageViews{};
        std::unordered_set<VkSemaphore> signalSemaphores{};
        // std::unordered_set<VkSemaphore> waitSemaphores{}; // not used.
        std::unordered_set<VkSampler> samplers{};
        std::unordered_set<VkPipeline> pipelines{};
        std::unordered_set<VkPipelineLayout> pipelineLayouts{};
        std::unordered_set<VkDescriptorSet> descriptorSet{};
        std::unordered_set<VkDescriptorSetLayout> descriptorSetLayouts{};
        std::unordered_set<VkFramebuffer> framebuffers{};
        std::unordered_set<VkRenderPass> renderPasses{};
    } object;
};

using VulkanInflightObjects = std::unordered_map<VkFence, VulkanInflight::Object>;

class VulkanDevice;
class CommandBuffer;
class VulkanInflightContext final
{
public:
    static VulkanInflight::Object generate(std::vector<CommandBuffer*> commandBuffers);

public:
    VulkanInflightContext() = delete;
    explicit VulkanInflightContext(VulkanDevice* device);
    ~VulkanInflightContext();

public:
    VkFence add(VkQueue queue, VulkanInflight::Object object);
    void clear(VkQueue queue);
    void clearAll();

private:
    [[maybe_unused]] VulkanDevice* m_device = nullptr;

private:
    std::unordered_map<VkQueue, InflightResources> m_resources{};
};

} // namespace jipu