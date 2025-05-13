#pragma once

#include "jipu/common/dylib.h"
#include "june/june.h"
#include "june_api.h"
#include "runner.h"

#include <filesystem>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

namespace jipu
{

struct JuneServiceDescriptor
{
    uint32_t fps{ 0 };
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    void* windowHandle{ nullptr };
    std::filesystem::path appPath;
    std::filesystem::path appDir;
    void* appHandle{ nullptr };
};

struct JuneServiceStartDescriptor
{
    std::function<void()> callback;
};

struct JuneServiceStopDescriptor
{
    std::function<void()> callback;
};

class JuneService
{
public:
    JuneService() = delete;
    JuneService(const JuneServiceDescriptor& descriptor);
    virtual ~JuneService();

public:
    void start(const JuneServiceStartDescriptor& descriptor);
    void stop(const JuneServiceStopDescriptor& descriptor);

public:
    virtual void addSharedMemory(JuneSharedMemory sharedMemory);
    virtual void addWaitFence(JuneFence fence);

    std::vector<JuneSharedMemory> getSharingMemories() const;
    JuneFence getSignalFence() const;

protected:
    virtual void begin();
    virtual void work();
    virtual void end();

    void addBeforeWork(const std::function<void()>& work);
    void addAfterWork(const std::function<void()>& work);

    void createInstance(const std::string& label);
    virtual void createApiContext(const std::string& label) = 0;

    std::vector<JuneFence> getWaitFences() const;
    std::vector<JuneSharedMemory> getSharedMemories() const;

private:
    void loadJuneLibrary();

    void start();
    void stop();

    virtual void beforeWork();
    virtual void afterWork();

protected:
    const JuneServiceDescriptor m_descriptor;
    JuneAPI m_juneAPI;

    JuneInstance m_juneInstance{ nullptr };
    JuneApiContext m_juneApiContext{ nullptr };

    std::vector<JuneSharedMemory> m_sharingMemories{};
    std::vector<JuneSharedMemory> m_sharedMemories{};

    JuneFence m_signalFence{ nullptr };
    std::vector<JuneFence> m_waitFences{};

    mutable std::mutex m_sharedMemoryMutex;
    mutable std::mutex m_waitFenceMutex;

private:
    Runner m_runner;

    DyLib m_juneLib;

    std::function<void()> m_startCallback;
    std::function<void()> m_stopCallback;

    std::queue<std::function<void()>> m_beforeWorks;
    std::queue<std::function<void()>> m_afterWorks;

    std::mutex m_beforeWorkMutex;
    std::mutex m_afterWorkMutex;
};

} // namespace jipu