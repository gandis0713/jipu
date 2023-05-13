#include "export.h"
#include "utils/cast.h"
#include "vkt/gpu/buffer.h"
#include "vulkan_api.h"
#include "vulkan_heap_memory.h"

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
    std::unique_ptr<VulkanHeapMemory> m_heapMemory{ nullptr };
};

DOWN_CAST(VulkanBuffer, Buffer);

} // namespace vkt