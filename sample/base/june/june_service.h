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

struct JuneServiceShareObjects
{
    JuneSharedMemory sharedMemory;
    std::vector<JuneFence> fences;
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
    virtual JuneServiceShareObjects getSharingObject() const = 0;
    virtual void setSharedObjects(const JuneServiceShareObjects& sharedObjects) = 0;

protected:
    virtual void begin();
    virtual void work();
    virtual void end();

    void addBeforeWork(const std::function<void()>& work);
    void addAfterWork(const std::function<void()>& work);

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

    JuneServiceShareObjects m_sharingObjects{};
    JuneServiceShareObjects m_sharedObjects{};

    mutable std::mutex m_sharedMutex;
    bool m_shared = false;

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