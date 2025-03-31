#include "june_service.h"

namespace jipu
{
JuneService::JuneService(const JuneServiceDescriptor& descriptor)
    : m_descriptor(descriptor)
    , m_runner(m_descriptor.fps)
{
}

JuneService::~JuneService()
{
    end();
}

void JuneService::start()
{
    loadJuneLibrary();

    m_runner.run(
        [this]() {
            begin();
        },
        [this]() {
            beforeWork();
            work();
            afterWork();
        },
        [this]() {
            end();
        });
}

void JuneService::stop()
{
    end();
}

void JuneService::begin()
{
    loadJuneLibrary();
}
void JuneService::beforeWork()
{
    // Prepare for work, e.g., set up resources
}
void JuneService::work()
{
    // Main work loop, e.g., rendering or processing
}
void JuneService::afterWork()
{
    // Clean up after work, e.g., release resources
}
void JuneService::end()
{
    if (m_runner.isRunning())
        m_runner.stop();

    if (m_juneLib.isValid())
        m_juneLib.close();
}

void JuneService::loadJuneLibrary()
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

} // namespace jipu