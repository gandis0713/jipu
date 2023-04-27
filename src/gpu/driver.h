#pragma once

#include "gpu/adapter.h"

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
    DRIVER_TYPE apiType = DRIVER_TYPE::NONE;
};

class Driver
{
public:
    static std::unique_ptr<Driver> create(DriverDescriptor descriptor);

public:
    virtual ~Driver() = default;

    Driver(const Driver&) = delete;
    Driver& operator=(const Driver&) = delete;

public:
    virtual std::unique_ptr<Adapter> createAdapter(AdapterDescriptor descriptor) = 0;

protected:
    Driver() = default;
};

} // namespace vkt
