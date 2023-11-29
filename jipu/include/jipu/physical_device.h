#pragma once

#include "export.h"
#include "jipu/device.h"
#include "jipu/surface.h"

#include <memory>

namespace jipu
{

class Driver;

struct PhysicalDeviceInfo
{
    std::string deviceName;
};

class JIPU_EXPORT PhysicalDevice
{
public:
    virtual ~PhysicalDevice() = default;

    virtual std::unique_ptr<Device> createDevice(DeviceDescriptor descriptor) = 0;

    virtual PhysicalDeviceInfo getInfo() const = 0;
    Driver* getDriver() const;

protected:
    PhysicalDevice(Driver* driver);
    friend class Driver;

protected:
    Driver* m_driver = nullptr;
};

} // namespace jipu
