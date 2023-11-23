#pragma once

#include "export.h"
#include "vkt/gpu/command_encoder.h"

#include <memory>

namespace jipu
{

enum class CommandBufferUsage
{
    kUndefined = 1 << 0, // 0x00000000
    kOneTime = 1 << 1,   // 0x00000001 // TODO: Check other graphic APi.
};

struct CommandBufferDescriptor
{
    CommandBufferUsage usage = CommandBufferUsage::kOneTime;
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
    CommandBufferUsage getUsage() const;

public:
    Device* getDevice() const;

protected:
    Device* m_device = nullptr;
    CommandBufferDescriptor m_descriptor = {};
};
} // namespace jipu