#pragma once

#include "export.h"
#include <functional>
#include <stdint.h>
#include <vector>

#include "binding_group_layout.h"

namespace jipu
{

using BindingGroupLayoutRef = std::reference_wrapper<BindingGroupLayout>;
using BindingGroupLayouts = std::vector<BindingGroupLayoutRef>;

class Device;
class BindingGroupLayout;
struct PipelineLayoutDescriptor
{
    BindingGroupLayouts layouts = {};
};

class JIPU_EXPORT PipelineLayout
{
public:
    virtual ~PipelineLayout() = default;

protected:
    PipelineLayout() = default;
};

} // namespace jipu