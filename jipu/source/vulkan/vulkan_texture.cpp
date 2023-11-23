#include "vulkan_texture.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanTexture::VulkanTexture(VulkanDevice* device, TextureDescriptor descriptor)
    : Texture(device, descriptor)
    , m_type(ToVkImageType(descriptor.type))
    , m_format(ToVkFormat(descriptor.format))
    , m_owner(TextureOwner::External)
{
    VkImageCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = ToVkImageType(descriptor.type);
    createInfo.extent.width = descriptor.width;
    createInfo.extent.height = descriptor.height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = descriptor.mipLevels;
    createInfo.arrayLayers = 1;
    createInfo.format = ToVkFormat(descriptor.format);
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.usage = ToVkImageUsageFlags(descriptor.usage);
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.samples = ToVkSampleCountFlagBits(descriptor.sampleCount);
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
    memoryDescriptor.flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT; // TODO: for only staging buffer.
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
    , m_owner(TextureOwner::Internal)
{
}

VulkanTexture::~VulkanTexture()
{
    if (m_owner == TextureOwner::External)
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

TextureOwner VulkanTexture::getTextureOwner() const
{
    return m_owner;
}

void VulkanTexture::setLayout(VkCommandBuffer commandBuffer, VkImageLayout layout, VkImageSubresourceRange range)
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
    barrier.subresourceRange = range;

    VkPipelineStageFlags srcStage = 0u;
    VkPipelineStageFlags dstStage = 0u;

    // TODO: generate barrier and stages. please refer or check https://harrylovescode.gitbooks.io/vulkan-api/content/chap07/chap07.html
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
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
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
    VkImageLayout layout = m_layout;

    if (layout == VK_IMAGE_LAYOUT_UNDEFINED)
    {
        if (m_owner == TextureOwner::Internal)
        {
            layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }
        else
        {
            layout = GenerateImageLayout(m_descriptor.usage);
        }
    }

    return layout;
}

// Convert Helper

VkFormat ToVkFormat(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::kBGRA_8888_UInt_Norm:
        return VK_FORMAT_B8G8R8A8_UNORM;
    case TextureFormat::kBGRA_8888_UInt_Norm_SRGB:
        return VK_FORMAT_B8G8R8A8_SRGB;
    case TextureFormat::kRGB_888_UInt_Norm:
        return VK_FORMAT_R8G8B8_UNORM;
    case TextureFormat::kRGB_888_UInt_Norm_SRGB:
        return VK_FORMAT_R8G8B8_SRGB;
    case TextureFormat::kRGBA_8888_UInt_Norm:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case TextureFormat::kRGBA_8888_UInt_Norm_SRGB:
        return VK_FORMAT_R8G8B8A8_SRGB;
    case TextureFormat::kRGBA_16161616_UInt_Norm:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    case TextureFormat::kD_32_SFloat:
        return VK_FORMAT_D32_SFLOAT;
    case TextureFormat::kD_24_UInt_Norm_S_8_UInt:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    default:
        assert_message(false, fmt::format("{} format does not support.", static_cast<uint32_t>(format)));
        return VK_FORMAT_UNDEFINED;
    }
}

TextureFormat ToTextureFormat(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_B8G8R8A8_UNORM:
        return TextureFormat::kBGRA_8888_UInt_Norm;
    case VK_FORMAT_B8G8R8A8_SRGB:
        return TextureFormat::kBGRA_8888_UInt_Norm_SRGB;
    case VK_FORMAT_R8G8B8_UNORM:
        return TextureFormat::kRGB_888_UInt_Norm;
    case VK_FORMAT_R8G8B8_SRGB:
        return TextureFormat::kRGB_888_UInt_Norm_SRGB;
    case VK_FORMAT_R8G8B8A8_UNORM:
        return TextureFormat::kRGBA_8888_UInt_Norm;
    case VK_FORMAT_R8G8B8A8_SRGB:
        return TextureFormat::kRGBA_8888_UInt_Norm_SRGB;
    default:
        assert_message(false, fmt::format("{} format does not support.", static_cast<uint32_t>(format)));
        return TextureFormat::kUndefined;
    }
}

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
    if (usages & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    {
        flags |= TextureUsageFlagBits::kCopyDst;
    }
    if (usages & VK_IMAGE_USAGE_SAMPLED_BIT)
    {
        flags |= TextureUsageFlagBits::kTextureBinding;
    }
    if (usages & VK_IMAGE_USAGE_STORAGE_BIT)
    {
        flags |= TextureUsageFlagBits::kStorageBinding;
    }
    if (usages & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        flags |= TextureUsageFlagBits::kDepthStencil;
    }
    if (usages & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
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
    if (usages & TextureUsageFlagBits::kCopyDst)
    {
        flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }
    if (usages & TextureUsageFlagBits::kTextureBinding)
    {
        flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    if (usages & TextureUsageFlagBits::kStorageBinding)
    {
        flags |= VK_IMAGE_USAGE_STORAGE_BIT;
    }
    if (usages & TextureUsageFlagBits::kDepthStencil)
    {
        flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    if (usages & TextureUsageFlagBits::kColorAttachment)
    {
        flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }

    return flags;
}

VkSampleCountFlagBits ToVkSampleCountFlagBits(uint32_t count)
{
    return count <= 1 ? VK_SAMPLE_COUNT_1_BIT : VK_SAMPLE_COUNT_4_BIT;
}

// Utiles

VkImageLayout GenerateImageLayout(TextureUsageFlags usage)
{
    if (usage & TextureUsageFlagBits::kTextureBinding)
    {
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
    if (usage & TextureUsageFlagBits::kColorAttachment)
    {
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    if (usage & TextureUsageFlagBits::kStorageBinding)
    {
        return VK_IMAGE_LAYOUT_GENERAL;
    }
    if (usage & TextureUsageFlagBits::kDepthStencil)
    {
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }
    if (usage & TextureUsageFlagBits::kCopySrc)
    {
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    }
    if (usage & TextureUsageFlagBits::kCopyDst)
    {
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    }

    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
}

} // namespace jipu
