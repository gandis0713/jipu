#pragma once

#include "vulkan_api.h"

#include <memory>
#include <unordered_map>

namespace vkt
{

struct VulkanRenderPassDescriptor
{
};

class VulkanDevice;
class VulkanRenderPass
{
public:
    VulkanRenderPass(VulkanDevice* device, VulkanRenderPassDescriptor descriptor);
    ~VulkanRenderPass();

    VkRenderPass getRenderPass() const;

private:
    VulkanDevice* m_device{ nullptr };

private:
    VkRenderPass m_renderPass{ nullptr };
};

class VulkanRenderPassCache final
{

public:
    VulkanRenderPassCache(VulkanDevice* device);
    ~VulkanRenderPassCache() = default;

    VulkanRenderPass* getRenderPass(const VulkanRenderPassDescriptor& descriptor);

private:
    VulkanDevice* m_device{ nullptr };

private:
    struct Functor
    {
        size_t operator()(const VulkanRenderPassDescriptor& descriptor) const;
        bool operator()(const VulkanRenderPassDescriptor& lhs, const VulkanRenderPassDescriptor& rhs) const;
    };
    using Cache = std::unordered_map<VulkanRenderPassDescriptor, std::unique_ptr<VulkanRenderPass>, Functor, Functor>;

    Cache m_cache{};
};

} // namespace vkt
