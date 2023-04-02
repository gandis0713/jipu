#pragma once

#include "gpu/device.h"
#include "gpu/platform.h"

#include <memory>

namespace vkt
{

struct AdapterDescriptor
{
    uint32_t index{ 0 }; // Index of physical device. If there is a device, index is 0.
};

class Driver;

class Adapter
{
public:
    Adapter() = delete;
    Adapter(Driver* driver, AdapterDescriptor descriptor);
    virtual ~Adapter() = default;

    virtual std::unique_ptr<Device> createDevice(DeviceDescriptor descriptor) = 0;
    virtual std::unique_ptr<Platform> createPlatform(PlatformDescriptor descriptor) = 0;

    Driver* getDriver() const;

protected:
    Driver* m_driver{ nullptr };
};

} // namespace vkt
