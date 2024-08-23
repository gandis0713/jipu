#pragma once

#include "export.h"
#include "jipu/command_encoder.h"

#include <memory>

namespace jipu
{

struct CommandBufferDescriptor
{
};

class Device;
class JIPU_EXPORT CommandBuffer
{
public:
    virtual ~CommandBuffer() = default;

    virtual std::unique_ptr<CommandEncoder> createCommandEncoder(const CommandEncoderDescriptor& descriptor) = 0;

public:
    using Ref = std::reference_wrapper<CommandBuffer>;
};
} // namespace jipu