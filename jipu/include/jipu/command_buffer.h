#pragma once

#include "export.h"
#include "jipu/command_encoder.h"

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
class JIPU_EXPORT CommandBuffer
{
public:
    virtual ~CommandBuffer() = default;

    virtual std::unique_ptr<CommandEncoder> createCommandEncoder(const CommandEncoderDescriptor& descriptor) = 0;

public:
    virtual CommandBufferUsage getUsage() const = 0;
};
} // namespace jipu