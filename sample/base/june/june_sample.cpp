#include "june_sample.h"

#include <algorithm>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <unordered_set>

#include "hpc/counter.h"
#include "hpc/gpu.h"

namespace jipu
{

JuneSample::JuneSample(const JuneSampleDescriptor& descriptor)
    : Window(descriptor.windowDescriptor)
    , m_appPath(descriptor.path)
    , m_appDir(descriptor.path.parent_path())
{
}

} // namespace jipu