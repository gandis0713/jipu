#pragma once

#include "gpu/adapter.h"

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
    virtual std::unique_ptr<Adapter> createAdapter(AdapterCreateInfo info) = 0;
};

} // namespace vkt
