#include "export.h"
#include "utils/cast.h"
#include "vkt/gpu/buffer.h"
#include "vulkan_api.h"
#include "vulkan_memory.h"

#include <memory>

namespace vkt
{

class VulkanDevice;
class VKT_EXPORT VulkanBuffer : public Buffer
{
public:
    VulkanBuffer() = delete;
    VulkanBuffer(VulkanDevice* device, const BufferDescriptor& descriptor) noexcept(false);
    ~VulkanBuffer() override;

    void* map() override;
    void unmap() override;

    VkBuffer getVkBuffer() const;

private:
    VkBuffer m_buffer{ VK_NULL_HANDLE };
    std::unique_ptr<VulkanMemory> m_memory{ nullptr };
};

DOWN_CAST(VulkanBuffer, Buffer);

// Convert Helper
VkAccessFlags BufferUsage2VkAccessFlags(BufferUsage usage);
BufferUsage VkAccessFlags2BufferUsage(VkAccessFlags flags);

} // namespace vkt