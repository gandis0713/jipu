#pragma once

#include "export.h"
#include <stdint.h>
#include <vector>

namespace jipu
{

class Device;
class BindingGroupLayout;
struct PipelineLayoutDescriptor
{
    std::vector<BindingGroupLayout*> layouts = {};
};

class VKT_EXPORT PipelineLayout
{
public:
    PipelineLayout() = delete;
    PipelineLayout(Device* device, const PipelineLayoutDescriptor& descriptor);
    virtual ~PipelineLayout() = default;

protected:
    Device* m_device = nullptr;
};

} // namespace jipu