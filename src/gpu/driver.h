#pragma once

#include "gpu/device.h"
#include "gpu/platform.h"

#include <memory>

namespace vkt
{

enum class API_TYPE
{
    NONE,
    VULKAN,
    METAL,
    D3D12
};

struct DriverCreateInfo
{
    API_TYPE apiType = API_TYPE::NONE;
};

class Driver
{
public:
    static std::unique_ptr<Driver> create(DriverCreateInfo info);

public:
    Driver() = default;
    virtual ~Driver() = default;

    Driver(const Driver&) = delete;
    Driver& operator=(const Driver&) = delete;

public:
    virtual std::unique_ptr<Device> createDevice(DeviceCreateInfo info) = 0;
    virtual std::unique_ptr<Platform> createPlatform(PlatformCreateInfo info) = 0;
};

} // namespace vkt
