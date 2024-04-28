#pragma once

#include "sampler.h"

#include <unordered_set>

namespace jipu
{
namespace hpc
{

class Counter
{
protected:
    Counter() = default;

public:
    enum Kind
    {
        GPUActiveCy
    };

public:
    virtual Sampler::Ptr create() = 0;

public:
    void addKind(Kind kind);

protected:
    std::unordered_set<Kind> m_kinds{};

public:
    using Ptr = std::unique_ptr<Counter>;
};

} // namespace hpc
} // namespace jipu