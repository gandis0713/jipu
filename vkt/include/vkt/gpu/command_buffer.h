#pragma once

namespace vkt
{

struct CommandBufferDescriptor
{
};

class CommandEncoder;
class CommandBuffer
{
public:
    CommandBuffer() = delete;
    CommandBuffer(CommandEncoder* encoder, const CommandBufferDescriptor& descriptor);
    virtual ~CommandBuffer() = default;

protected:
    CommandEncoder* m_encoder = nullptr;
};
} // namespace vkt