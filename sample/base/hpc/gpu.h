#pragma once

#include <memory>

namespace jipu
{
namespace hpc
{

class GPU
{
public:
    GPU();

public:
    using Ptr = std::unique_ptr<GPU>;
};

} // namespace hpc
} // namespace jipu