#include "vulkan_texture.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace vkt
{

VulkanTexture::VulkanTexture(VulkanDevice* device, TextureDescriptor descriptor)
    : Texture(device, descriptor)
    , m_type(ToVkImageType(descriptor.type))
    , m_format(ToVkFormat(descriptor.format))
    , m_owner(TextureOwner::Internal)
{
    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = ToVkImageType(descriptor.type);
    createInfo.extent.width = static_cast<uint32_t>(descriptor.width);
    createInfo.extent.height = static_cast<uint32_t>(descriptor.height);
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.arrayLayers = 1;
    createInfo.format = ToVkFormat(descriptor.format);
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.flags = 0; // Optional

    const VulkanAPI& vkAPI = device->vkAPI;
    VkResult result = vkAPI.CreateImage(device->getVkDevice(), &createInfo, nullptr, &m_image);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("Failed to create image. {}", static_cast<uint32_t>(result)));
    }

    VkMemoryRequirements memoryRequirements{};
    vkAPI.GetImageMemoryRequirements(device->getVkDevice(), m_image, &memoryRequirements);

    VulkanMemoryDescriptor memoryDescriptor{};
    memoryDescriptor.flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    memoryDescriptor.requirements = memoryRequirements;

    m_memory = std::make_unique<VulkanMemory>(device, memoryDescriptor);

    result = vkAPI.BindImageMemory(device->getVkDevice(), m_image, m_memory->getVkDeviceMemory(), 0);
    if (result != VK_SUCCESS)
    {
        // TODO: delete VkImage resource automatically.
        device->vkAPI.DestroyImage(device->getVkDevice(), m_image, nullptr);
        throw std::runtime_error(fmt::format("Failed to bind memory. {}", static_cast<int32_t>(result)));
    }
}

VulkanTexture::VulkanTexture(VulkanDevice* device, VkImage image, TextureDescriptor descriptor)
    : Texture(device, descriptor)
    , m_image(image)
    , m_type(ToVkImageType(descriptor.type))
    , m_format(ToVkFormat(descriptor.format))
    , m_owner(TextureOwner::External)
{
}

VulkanTexture::~VulkanTexture()
{
    if (m_owner == TextureOwner::Internal)
    {
        m_memory.reset();

        VulkanDevice* vulkanDevice = downcast(m_device);
        vulkanDevice->vkAPI.DestroyImage(vulkanDevice->getVkDevice(), m_image, nullptr);
    }
}

std::unique_ptr<TextureView> VulkanTexture::createTextureView(const TextureViewDescriptor& descriptor)
{
    return std::make_unique<VulkanTextureView>(this, descriptor);
}

VkImage VulkanTexture::getVkImage() const
{
    return m_image;
}

void VulkanTexture::setLayout(VkImageLayout layout, VkCommandBuffer commandBuffer)
{
    VkImageLayout oldLayout = m_layout;
    VkImageLayout newLayout = layout;

    if (oldLayout == newLayout)
    {
        spdlog::debug("old layout and new layout are same.");
        return;
    }

    auto vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    // set Image Memory Barrier
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage = 0u;
    VkPipelineStageFlags dstStage = 0u;

    // TODO: generate barrier and stages.
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        throw std::invalid_argument("Unsupported layout transition.");
    }
    vkAPI.CmdPipelineBarrier(
        commandBuffer,
        srcStage, dstStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    // set current layout.
    m_layout = layout;
}
VkImageLayout VulkanTexture::getLayout() const
{
    return m_layout;
}

// Convert Helper
VkImageType ToVkImageType(TextureType type)
{
    switch (type)
    {
    case TextureType::k1D:
        return VK_IMAGE_TYPE_1D;
    case TextureType::k2D:
        return VK_IMAGE_TYPE_2D;
    case TextureType::k3D:
        return VK_IMAGE_TYPE_3D;
    default:
        assert_message(false, fmt::format("{} type does not support.", static_cast<uint32_t>(type)));
        return VK_IMAGE_TYPE_1D;
    }
}

TextureType ToTextureType(VkImageType type)
{
    switch (type)
    {
    case VK_IMAGE_TYPE_1D:
        return TextureType::k1D;
    case VK_IMAGE_TYPE_2D:
        return TextureType::k2D;
    case VK_IMAGE_TYPE_3D:
        return TextureType::k3D;
    default:

        assert_message(false, fmt::format("{} type does not support.", static_cast<uint32_t>(type)));
        return TextureType::kUndefined;
    }
}

TextureUsageFlags ToTextureUsageFlags(VkImageUsageFlags usages)
{
    TextureUsageFlags flags = 0u;

    if (usages & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
    {
        flags |= TextureUsageFlagBits::kCopySrc;
    }
    else if (usages & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    {
        flags |= TextureUsageFlagBits::kCopyDst;
    }
    else if (usages & VK_IMAGE_USAGE_SAMPLED_BIT)
    {
        flags |= TextureUsageFlagBits::kTextureBinding;
    }
    else if (usages & VK_IMAGE_USAGE_STORAGE_BIT)
    {
        flags |= TextureUsageFlagBits::kStorageBinding;
    }
    else if (usages & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        flags |= TextureUsageFlagBits::kDepthStencil;
    }
    else if (usages & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
    {
        flags |= TextureUsageFlagBits::kColorAttachment;
    }

    return flags;
}

VkImageUsageFlags ToVkImageUsageFlags(TextureUsageFlags usages)
{
    VkImageUsageFlags flags = 0u;

    if (usages & TextureUsageFlagBits::kCopySrc)
    {
        flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    else if (usages & TextureUsageFlagBits::kCopyDst)
    {
        flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }
    else if (usages & TextureUsageFlagBits::kTextureBinding)
    {
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    else if (usages & TextureUsageFlagBits::kStorageBinding)
    {
        flags |= VK_IMAGE_USAGE_STORAGE_BIT;
    }
    else if (usages & TextureUsageFlagBits::kDepthStencil)
    {
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    else if (usages & TextureUsageFlagBits::kColorAttachment)
    {
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    return flags;
}

} // namespace vkt
