#pragma once

#include <cstdint>
#include <memory>
#include <unordered_set>
#include <vector>

#include "counter.h"
#include "export.h"

namespace hpc
{

struct Sample
{
    enum class Type : uint8_t
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

    hpc::Counter counter;
    uint64_t timestamp{};
    Value value{ static_cast<uint64_t>(0) };
    Type type{ Type::uint64 };
};

struct SamplerDescriptor
{
    std::unordered_set<hpc::Counter> counters{};
};

class HPC_EXPORT Sampler
{
public:
    virtual ~Sampler() = default;

protected:
    Sampler() = default;

public:
    virtual void start() = 0;
    virtual void stop() = 0;

    /**
     * @brief get samples with counters
     */
    virtual std::vector<Sample> samples(std::unordered_set<Counter> counters = {}) = 0;

public:
    const std::unordered_set<Counter>& counters() const;
};

} // namespace hpc