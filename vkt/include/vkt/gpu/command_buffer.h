#pragma once

#include "export.h"
#include "vkt/gpu/command_encoder.h"

#include <memory>

namespace vkt
{

struct CommandBufferDescriptor
{
};

class Device;
class VKT_EXPORT CommandBuffer
{
public:
    CommandBuffer() = delete;
    CommandBuffer(Device* device, const CommandBufferDescriptor& descriptor);
    virtual ~CommandBuffer() = default;

    virtual std::unique_ptr<CommandEncoder> createCommandEncoder(const CommandEncoderDescriptor& descriptor) = 0;

public:
    Device* getDevice() const;

protected:
    Device* m_device = nullptr;
};
} // namespace vkt