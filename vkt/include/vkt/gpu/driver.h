#pragma once

#include "export.h"
#include "vkt/gpu/physical_device.h"

#include <memory>

namespace vkt
{

enum class DRIVER_TYPE
{
    NONE,
    VULKAN,
    METAL,
    D3D12
};

struct DriverDescriptor
{
    DRIVER_TYPE type = DRIVER_TYPE::NONE;
};

class VKT_EXPORT Driver
{
public:
    static std::unique_ptr<Driver> create(DriverDescriptor descriptor);

public:
    virtual ~Driver() = default;

    Driver(const Driver&) = delete;
    Driver& operator=(const Driver&) = delete;

public:
    virtual std::unique_ptr<PhysicalDevice> createPhysicalDevice(PhysicalDeviceDescriptor descriptor) = 0;
    virtual std::unique_ptr<Surface> createSurface(SurfaceDescriptor descriptor) = 0;

protected:
    Driver() = default;
};

} // namespace vkt
