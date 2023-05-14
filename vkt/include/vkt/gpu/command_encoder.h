#pragma once

namespace vkt
{

struct CommandEncoderDescriptor
{
};

class Device;
class CommandEncoder
{
public:
    CommandEncoder() = delete;
    CommandEncoder(Device* device, const CommandEncoderDescriptor& descriptor);
    virtual ~CommandEncoder() = default;

protected:
    Device* m_device = nullptr;
};
} // namespace vkt