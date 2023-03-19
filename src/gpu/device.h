#pragma once

namespace vkt
{

struct DeviceCreateInfo
{
};

class Device
{
public:
    Device() = delete;
    Device(DeviceCreateInfo info);
    virtual ~Device() = default;

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;
};

} // namespace vkt
