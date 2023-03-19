#pragma once

#include "gpu/device.h"
#include "gpu/platform.h"

#include <memory>

namespace vkt
{

struct AdapterCreateInfo
{
};

class Driver;

class Adapter
{
public:
    Adapter() = delete;
    Adapter(Driver* driver, AdapterCreateInfo info);
    virtual ~Adapter() = default;

    virtual std::unique_ptr<Device> createDevice(DeviceCreateInfo info) = 0;
    virtual std::unique_ptr<Platform> createPlatform(PlatformCreateInfo info) = 0;

    Driver* getDriver() const;

protected:
    Driver* m_driver{ nullptr };
};

} // namespace vkt
