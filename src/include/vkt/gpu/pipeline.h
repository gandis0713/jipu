#pragma once

#include "export.h"

namespace vkt
{

class Device;

struct PipelineDescriptor
{
};

class VKT_EXPORT Pipeline
{
public:
    Pipeline() = delete;
    Pipeline(Device* device, const PipelineDescriptor& descriptor);
    virtual ~Pipeline() = default;

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

protected:
    Device* m_device{ nullptr };
};

} // namespace vkt
