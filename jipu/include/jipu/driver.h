#pragma once

#include "export.h"
#include "jipu/physical_device.h"
#include "jipu/surface.h"

#include <memory>

namespace jipu
{

enum class DriverType
{
    kNone,
    kVulkan,
    kMetal,
    kD3D12
};

struct DriverDescriptor
{
    DriverType type = DriverType::kNone;
};

class JIPU_EXPORT Driver
{
public:
    static std::unique_ptr<Driver> create(const DriverDescriptor& descriptor);

public:
    virtual ~Driver();

    Driver(const Driver&) = delete;
    Driver& operator=(const Driver&) = delete;

protected:
    Driver();

public:
    virtual std::vector<std::unique_ptr<PhysicalDevice>> getPhysicalDevices() = 0;
    virtual std::unique_ptr<Surface> createSurface(const SurfaceDescriptor& descriptor) = 0;
};

} // namespace jipu
