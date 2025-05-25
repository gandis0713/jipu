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

void JuneSample::loadJuneLibrary()
{
    std::string juneLibName;
#if defined(__ANDROID__) || defined(ANDROID)
    juneLibName = "libjune.so";
#elif defined(__linux__)
    juneLibName = "libjune.so";
#elif defined(__APPLE__)
    juneLibName = "libjune.dylib";
#elif defined(WIN32)
    juneLibName = "june.dll";
#endif
    if (!m_juneLib.open(juneLibName.c_str()))
    {
        throw std::runtime_error("Failed to open library");
    }
    if (!m_juneAPI.loadProcs(&m_juneLib))
    {
        throw std::runtime_error("Failed to load procs");
    }
}

void JuneSample::createInstance(const std::string& label)
{
    JuneInstanceDescriptor juneInstanceDescriptor{};
    juneInstanceDescriptor.label.data = label.data();
    juneInstanceDescriptor.label.length = static_cast<uint32_t>(label.length());

    m_juneInstance = m_juneAPI.CreateInstance(&juneInstanceDescriptor);
    if (!m_juneInstance)
    {
        spdlog::error("Failed to create June instance with label: {}", label);
        return;
    }
}

} // namespace jipu