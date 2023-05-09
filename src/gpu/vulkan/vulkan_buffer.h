#include "export.h"
#include "utils/cast.h"
#include "vkt/gpu/buffer.h"
#include "vulkan_api.h"

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
    VkDeviceMemory m_deviceMemory{ VK_NULL_HANDLE };

    void* m_mappedPointer{ nullptr };
};

DOWN_CAST(VulkanBuffer, Buffer);

} // namespace vkt