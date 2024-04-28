#pragma once

#include <memory>
#include <stdint.h>

#include "counter.h"

namespace jipu
{
namespace hpc
{

class GPU
{
protected:
    GPU() = default;

public:
    virtual Counter::Ptr create() = 0;

public:
    using Ptr = std::unique_ptr<GPU>;
};

} // namespace hpc
} // namespace jipu