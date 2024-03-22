#pragma once

#include "export.h"
#include <functional>
#include <stdint.h>
#include <vector>

namespace jipu
{

class Device;
class BindingGroupLayout;
struct PipelineLayoutDescriptor
{
    std::vector<std::reference_wrapper<BindingGroupLayout>> layouts = {};
};

class JIPU_EXPORT PipelineLayout
{
public:
    virtual ~PipelineLayout() = default;

protected:
    PipelineLayout() = default;
};

} // namespace jipu