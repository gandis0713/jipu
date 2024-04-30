#pragma once

#include <memory>
#include <vector>

#include "counter.h"
#include "export.h"

#include <cstdint>

namespace jipu
{
namespace hpc
{

struct Sample
{
    enum class Type
    {
        uint64,
        float64
    };

    union Value {
        uint64_t uint64;
        double float64;

        explicit Value(uint64_t v)
            : uint64(v)
        {
        }

        explicit Value(double v)
            : float64(v)
        {
        }
    };

    Counter counter;
    uint64_t timestamp{};
    Value value{ static_cast<uint64_t>(0) };
    Type type{ Type::uint64 };
};

struct SamplerDescriptor
{
    std::vector<Counter> counters{};
};

class HPC_EXPORT Sampler
{
protected:
    Sampler(SamplerDescriptor descriptor);

public:
    virtual Sample getSample(Counter counter) = 0;

public:
    const std::vector<Counter>& counters() const;

public:
    using Ptr = std::unique_ptr<Sampler>;

private:
    SamplerDescriptor m_descriptor{};
};

} // namespace hpc
} // namespace jipu