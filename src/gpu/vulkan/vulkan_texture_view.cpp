#include "vulkan_texture_view.h"
#include "vulkan_texture.h"

namespace vkt
{

VulkanTextureView::VulkanTextureView(VulkanTexture* texture, TextureViewCreateInfo info) : TextureView(texture, info) {}

} // namespace vkt