#pragma once

#include <memory>

namespace jipu
{
namespace hpc
{

class Sampler
{
protected:
    Sampler() = default;

public:
    using Ptr = std::unique_ptr<Sampler>;
};

} // namespace hpc
} // namespace jipu