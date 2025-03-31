#include "june_service.h"

namespace jipu
{
JuneService::JuneService(const JuneServiceDescriptor& descriptor)
    : m_descriptor(descriptor)
    , m_runner(m_descriptor.fps)
{
    loadJuneLibrary();
}

JuneService::~JuneService()
{
    stop();

    if (m_juneLib.isValid())
        m_juneLib.close();
}

void JuneService::start(const JuneServiceStartDescriptor& descriptor)
{
    m_startCallback = descriptor.callback;
    start();
}

void JuneService::stop(const JuneServiceStopDescriptor& descriptor)
{
    m_stopCallback = descriptor.callback;

    stop();

    if (m_stopCallback)
        m_stopCallback();
}

JuneSharedMemory JuneService::getJuneSharedMemory(const std::string& key) const
{
    return m_sharedMemories.contains(key) ? m_sharedMemories.at(key) : nullptr;
}

void JuneService::setJuneSharedMemory(const std::string& key, JuneSharedMemory juneSharedMemory)
{
    if (juneSharedMemory)
    {
        m_sharedMemories[key] = juneSharedMemory;
    }
    else
    {
        m_sharedMemories.erase(key);
    }
}

void JuneService::shareMemory(JuneSharedMemory juneSharedMemory)
{
}

void JuneService::begin()
{
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
}

void JuneService::start()
{
    m_runner.run(
        [this]() {
            begin();
            if (m_startCallback)
                m_startCallback();
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
    if (m_runner.isRunning())
        m_runner.stop();
}

void JuneService::addBeforeWork(const std::function<void()>& work)
{
    std::lock_guard<std::mutex> lock(m_beforeWorkMutex);
    m_beforeWorks.push(work);
}

void JuneService::addAfterWork(const std::function<void()>& work)
{
    std::lock_guard<std::mutex> lock(m_afterWorkMutex);
    m_afterWorks.push(work);
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