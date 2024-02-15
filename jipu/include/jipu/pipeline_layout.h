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

class JIPU_EXPORT PipelineLayout
{
public:
    virtual ~PipelineLayout() = default;

protected:
    PipelineLayout() = default;
};

} // namespace jipu