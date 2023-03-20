#pragma once

namespace vkt
{

class Device;

struct FramebufferCreateInfo
{
};

class FrameBuffer
{
public:
    FrameBuffer() = delete;
    FrameBuffer(Device* device, FramebufferCreateInfo info);
    virtual ~FrameBuffer() = default;

    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

protected:
    Device* m_device{ nullptr };
};

} // namespace vkt