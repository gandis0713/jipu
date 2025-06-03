#pragma once

#include "jipu/common/dylib.h"
#include "june/june.h"
#include "june_api.h"
#include "june_memory_node.h"
#include "runner.h"

#include <filesystem>
#include <functional>
#include <memory>
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
    void pause();
    void resume();
    void stop(const JuneServiceStopDescriptor& descriptor);

public:
    virtual void addSharedMemory(JuneSharedMemory sharedMemory);
    virtual void addWaitFence(JuneFence fence);
    JuneFence getSignalFence() const;
    JuneMemoryNode* getMemoryNode() const;
    void connectMemoryNode(JuneMemoryNode* inputNode);
    void disconnectMemoryNode(JuneMemoryNode* inputNode);

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
    std::vector<JuneSharedMemory> m_sharedMemories{};
    std::unique_ptr<JuneMemoryNode> m_memoryNode{ nullptr };
    JuneFence m_signalFence{ nullptr };
    std::vector<JuneFence> m_waitFences{};

    mutable std::mutex m_sharedMemoryMutex;
    mutable std::mutex m_waitFenceMutex;
    mutable std::mutex m_memoryNodeMutex;

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