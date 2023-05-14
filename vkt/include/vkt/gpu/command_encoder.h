#pragma once

#include "export.h"

namespace vkt
{

struct CommandEncoderDescriptor
{
};

class CommandBuffer;
class CommandEncoder
{
public:
    CommandEncoder() = delete;
    CommandEncoder(CommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor);
    virtual ~CommandEncoder() = default;

protected:
    CommandBuffer* m_commandBuffer = nullptr;
};
} // namespace vkt