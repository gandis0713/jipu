#include "vulkan_texture.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_resource_allocator.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanTextureDescriptor generateVulkanDescriptor(const TextureDescriptor& descriptor)
{
    VulkanTextureDescriptor vkdescriptor{};

    vkdescriptor.imageType = ToVkImageType(descriptor.type);
    vkdescriptor.extent.width = descriptor.width;
    vkdescriptor.extent.height = descriptor.height;
    vkdescriptor.extent.depth = descriptor.depth;
    vkdescriptor.mipLevels = descriptor.mipLevels;
    vkdescriptor.arrayLayers = 1;
    vkdescriptor.format = ToVkFormat(descriptor.format);
    vkdescriptor.tiling = VK_IMAGE_TILING_OPTIMAL;
    vkdescriptor.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vkdescriptor.usage = ToVkImageUsageFlags(descriptor.usage);
    vkdescriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkdescriptor.samples = ToVkSampleCountFlagBits(descriptor.sampleCount);
    vkdescriptor.flags = 0;

    return vkdescriptor;
}

VulkanTexture::VulkanTexture(VulkanDevice* device, const TextureDescriptor& descriptor)
    : VulkanTexture(device, generateVulkanDescriptor(descriptor))
{
}

VulkanTexture::VulkanTexture(VulkanDevice* device, const VulkanTextureDescriptor& descriptor)
    : m_device(device)
    , m_descriptor(descriptor)
{
    if (m_descriptor.extent.width == 0 || m_descriptor.extent.height == 0 || m_descriptor.extent.depth == 0)
    {
        throw std::runtime_error("Texture size must be greater than 0.");
    }

    if (m_descriptor.usage == 0u)
    {
        throw std::runtime_error("Texture usage must not be undefined.");
    }

    if (m_descriptor.format == VK_FORMAT_UNDEFINED)
    {
        throw std::runtime_error("Texture format must not be undefined.");
    }

    if (m_descriptor.image == VK_NULL_HANDLE)
    {
        VkImageCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        createInfo.imageType = m_descriptor.imageType;
        createInfo.extent.width = m_descriptor.extent.width;
        createInfo.extent.height = m_descriptor.extent.height;
        createInfo.extent.depth = m_descriptor.extent.depth;
        createInfo.mipLevels = m_descriptor.mipLevels;
        createInfo.arrayLayers = m_descriptor.arrayLayers;
        createInfo.format = m_descriptor.format;
        createInfo.tiling = m_descriptor.tiling;
        createInfo.initialLayout = m_descriptor.initialLayout;
        createInfo.usage = m_descriptor.usage;
        createInfo.sharingMode = m_descriptor.sharingMode;
        createInfo.samples = m_descriptor.samples;
        createInfo.flags = m_descriptor.flags;

        auto vulkanResourceAllocator = device->getResourceAllocator();
        m_resource = vulkanResourceAllocator->createTexture(createInfo);

        m_owner = VulkanTextureOwner::User;
    }
    else
    {
        m_resource.image = m_descriptor.image;
        m_owner = VulkanTextureOwner::Swapchain;
    }
}

VulkanTexture::~VulkanTexture()
{
    if (m_owner == VulkanTextureOwner::User)
    {
        auto vulkanResourceAllocator = downcast(m_device)->getResourceAllocator();
        vulkanResourceAllocator->destroyTexture(m_resource);
    }
}

std::unique_ptr<TextureView> VulkanTexture::createTextureView(const TextureViewDescriptor& descriptor)
{
    return std::make_unique<VulkanTextureView>(this, descriptor);
}

TextureType VulkanTexture::getType() const
{
    return ToTextureType(m_descriptor.imageType);
}

TextureFormat VulkanTexture::getFormat() const
{
    return ToTextureFormat(m_descriptor.format);
}

TextureUsageFlags VulkanTexture::getUsage() const
{
    return ToTextureUsageFlags(m_descriptor.usage);
}

uint32_t VulkanTexture::getWidth() const
{
    return m_descriptor.extent.width;
}

uint32_t VulkanTexture::getHeight() const
{
    return m_descriptor.extent.height;
}

uint32_t VulkanTexture::getDepth() const
{
    return m_descriptor.extent.depth;
}

uint32_t VulkanTexture::getMipLevels() const
{
    return m_descriptor.mipLevels;
}

uint32_t VulkanTexture::getSampleCount() const
{
    return ToSampleCount(m_descriptor.samples);
}

VulkanDevice* VulkanTexture::getDevice() const
{
    return m_device;
}

VkImage VulkanTexture::getVkImage() const
{
    return m_resource.image;
}

void VulkanTexture::setPipelineBarrier(VkCommandBuffer commandBuffer, VkImageLayout layout, VkImageSubresourceRange range)
{
    if (commandBuffer == VK_NULL_HANDLE)
        throw std::runtime_error("Command buffer is null handle to set pipeline barrier in texture.");

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
    barrier.pNext = VK_NULL_HANDLE;
    barrier.srcAccessMask = GenerateAccessFlags(oldLayout);
    barrier.dstAccessMask = GenerateAccessFlags(newLayout);
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.image = m_resource.image;
    barrier.subresourceRange = range;

    VkPipelineStageFlags srcStage = GeneratePipelineStage(oldLayout);
    VkPipelineStageFlags dstStage = GeneratePipelineStage(newLayout);

    vkAPI.CmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    // set current layout.
    m_layout = layout;
}

VkImageLayout VulkanTexture::getLayout() const
{
    return m_layout;
}

VkImageLayout VulkanTexture::getFinalLayout() const
{
    switch (m_owner)
    {
    case VulkanTextureOwner::Swapchain:
        return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    default:
    case VulkanTextureOwner::User:
        return GenerateFinalImageLayout(m_descriptor.usage);
    }
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
        return VK_FORMAT_R16G16B16A16_UNORM;
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
    case VK_FORMAT_R16G16B16A16_UNORM:
        return TextureFormat::kRGBA_16161616_UInt_Norm;
    case VK_FORMAT_D32_SFLOAT:
        return TextureFormat::kD_32_SFloat;
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return TextureFormat::kD_24_UInt_Norm_S_8_UInt;
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

uint32_t ToSampleCount(VkSampleCountFlagBits flag)
{
    if (flag == 0u)
    {
        throw std::runtime_error("sample count flag is undefined.");
    }

    return flag == VK_SAMPLE_COUNT_1_BIT ? 1 : 4;
}

// Utiles

VkImageLayout GenerateFinalImageLayout(VkImageUsageFlags usage)
{
    if (usage & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT || usage & VK_IMAGE_USAGE_SAMPLED_BIT)
    {
        return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
    if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
    {
        return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    if (usage & VK_IMAGE_USAGE_STORAGE_BIT)
    {
        return VK_IMAGE_LAYOUT_GENERAL;
    }
    if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }
    if (usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
    {
        return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    }
    if (usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    {
        return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    }

    return VK_IMAGE_LAYOUT_UNDEFINED;
}

// VkImageLayout GenerateFinalImageLayout(TextureUsageFlags usage)
// {
//     if (usage & TextureUsageFlagBits::kTextureBinding)
//     {
//         return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//     }
//     if (usage & TextureUsageFlagBits::kColorAttachment)
//     {
//         return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//     }
//     if (usage & TextureUsageFlagBits::kStorageBinding)
//     {
//         return VK_IMAGE_LAYOUT_GENERAL;
//     }
//     if (usage & TextureUsageFlagBits::kDepthStencil)
//     {
//         return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//     }
//     if (usage & TextureUsageFlagBits::kCopySrc)
//     {
//         return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//     }
//     if (usage & TextureUsageFlagBits::kCopyDst)
//     {
//         return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//     }

//     return VK_IMAGE_LAYOUT_UNDEFINED;
// }

VkAccessFlags GenerateAccessFlags(VkImageLayout layout)
{
    VkAccessFlags accessFlags = 0x0u;

    switch (layout)
    {
    default:
    case VK_IMAGE_LAYOUT_UNDEFINED:
        accessFlags = VK_ACCESS_NONE;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        accessFlags = VK_ACCESS_TRANSFER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        accessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        accessFlags = VK_ACCESS_SHADER_READ_BIT;
        break;
    }

    return accessFlags;
}

VkPipelineStageFlags GeneratePipelineStage(VkImageLayout layout)
{
    VkPipelineStageFlags pipelineStage = 0x0u;

    switch (layout)
    {
    default:
    case VK_IMAGE_LAYOUT_UNDEFINED:
        pipelineStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        pipelineStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        pipelineStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        break;
    }

    return pipelineStage;
}

} // namespace jipu
