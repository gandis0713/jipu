#pragma once

namespace jipu
{
namespace hpc
{

enum class Counter
{
    NonFragmentUtilization = 0,
    FragmentUtilization,
    TilerUtilization,
    ExternalReadBytes,
    ExternalWriteBytes,
    ExternalReadStallRate,
    ExternalWriteStallRate,
    ExternalReadLatency0,
    ExternalReadLatency1,
    ExternalReadLatency2,
    ExternalReadLatency3,
    ExternalReadLatency4,
    ExternalReadLatency5,
    GeometryTotalInputPrimitives,
    Count // total enum count
};

const char* name(Counter counter);

} // namespace hpc
} // namespace jipu