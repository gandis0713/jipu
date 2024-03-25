#pragma once

#include "export.h"
#include "jipu/device.h"
#include "jipu/surface.h"

#include <memory>

namespace jipu
{

struct PhysicalDeviceInfo
{
    std::string deviceName;
};

class Driver;
class JIPU_EXPORT PhysicalDevice
{
public:
    virtual ~PhysicalDevice() = default;

protected:
    PhysicalDevice() = default;

public:
    virtual std::unique_ptr<Device> createDevice(const DeviceDescriptor& descriptor) = 0;

public:
    virtual PhysicalDeviceInfo getInfo() const = 0;
};

} // namespace jipu
