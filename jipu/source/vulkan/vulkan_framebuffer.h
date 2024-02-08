#pragma once

#include "export.h"

#include <memory>
#include <unordered_map>
#include <vector>

#include "vulkan_api.h"
#include "vulkan_render_pass.h"

namespace jipu
{

struct VulkanFramebufferDescriptor
{
    VulkanRenderPass* renderPass = nullptr;
    std::vector<TextureView*> textureViews{};
};

class VulkanDevice;
class JIPU_EXPERIMENTAL_EXPORT VulkanFrameBuffer
{
public:
    VulkanFrameBuffer() = delete;
    VulkanFrameBuffer(VulkanDevice* device, const VulkanFramebufferDescriptor& descriptor);
    ~VulkanFrameBuffer();

    VkFramebuffer getVkFrameBuffer() const;

    uint32_t getWidth() const;
    uint32_t getHeight() const;

private:
    VulkanDevice* m_device = nullptr;

private:
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

    uint32_t m_width = 0;
    uint32_t m_height = 0;
};

class JIPU_EXPERIMENTAL_EXPORT VulkanFrameBufferCache final
{

public:
    VulkanFrameBufferCache(VulkanDevice* device);
    ~VulkanFrameBufferCache() = default;

    VulkanFrameBuffer* getFrameBuffer(const VulkanFramebufferDescriptor& descriptor);

    void clear();

private:
    VulkanDevice* m_device = nullptr;

private:
    struct Functor
    {
        // hash key
        size_t operator()(const VulkanFramebufferDescriptor& descriptor) const;
        // equal
        bool operator()(const VulkanFramebufferDescriptor& lhs, const VulkanFramebufferDescriptor& rhs) const;
    };
    using Cache = std::unordered_map<VulkanFramebufferDescriptor, std::unique_ptr<VulkanFrameBuffer>, Functor, Functor>;

    Cache m_cache{};
};

} // namespace jipu
