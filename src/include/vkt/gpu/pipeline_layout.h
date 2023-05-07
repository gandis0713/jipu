#pragma once

#include "export.h"

namespace vkt
{

struct PipelineLayoutDescriptor
{
};

class Device;
class VKT_EXPORT PipelineLayout
{
public:
    PipelineLayout() = delete;
    PipelineLayout(Device* device, const PipelineLayoutDescriptor& descriptor);
    virtual ~PipelineLayout() = default;

protected:
    Device* m_device{ nullptr };
};

} // namespace vkt