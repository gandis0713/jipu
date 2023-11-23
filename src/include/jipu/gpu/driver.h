#pragma once

#include "export.h"
#include "jipu/gpu/physical_device.h"
#include "jipu/gpu/surface.h"

#include <memory>

namespace jipu
{

enum class DriverType
{
    NONE,
    VULKAN,
    METAL,
    D3D12
};

struct DriverDescriptor
{
    DriverType type = DriverType::NONE;
};

class JIPU_EXPORT Driver
{
public:
    static std::unique_ptr<Driver> create(const DriverDescriptor& descriptor);

public:
    Driver() = delete;
    Driver(const DriverDescriptor& descriptor);
    virtual ~Driver() = default;

    Driver(const Driver&) = delete;
    Driver& operator=(const Driver&) = delete;

public:
    virtual std::unique_ptr<PhysicalDevice> createPhysicalDevice(const PhysicalDeviceDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Surface> createSurface(const SurfaceDescriptor& descriptor) = 0;
};

} // namespace jipu
