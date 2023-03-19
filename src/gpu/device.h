#pragma once

namespace vkt
{

struct DeviceCreateInfo
{
};

class Adapter;

class Device
{
public:
    Device() = delete;
    Device(Adapter* adapter, DeviceCreateInfo info);
    virtual ~Device() = default;

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

protected:
    Adapter* m_adapter;
};

} // namespace vkt
