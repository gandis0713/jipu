#pragma once

#include "counter.h"

namespace jipu
{
namespace hpc
{

class MaliCounter : public Counter
{
public:
    explicit MaliCounter(std::unordered_set<Type> types);
};

} // namespace hpc
} // namespace jipu