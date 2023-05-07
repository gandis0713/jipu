#include "vkt/gpu/buffer.h"

namespace vkt
{

class VulkanDevice;
class VulkanBuffer : public Buffer
{
public:
    VulkanBuffer() = delete;
    VulkanBuffer(VulkanDevice* device, const BufferDescriptor& descriptor);
    ~VulkanBuffer() override = default;
};

} // namespace vkt