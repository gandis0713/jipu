#pragma once

#include "jipu/common/dylib.h"
#include "june/june.h"
#include "june_api.h"
#include "runner.h"

#include <functional>

namespace jipu
{

struct JuneServiceDescriptor
{
    uint32_t fps{ 0 };
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    void* windowHandle{ nullptr };
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
    virtual JuneSharedMemory getJuneSharedMemory() const;

protected:
    virtual void begin();
    virtual void beforeWork();
    virtual void work();
    virtual void afterWork();
    virtual void end();

    void start();
    void stop();

protected:
    void loadJuneLibrary();

protected:
    const JuneServiceDescriptor m_descriptor;

    Runner m_runner;

    DyLib m_juneLib;
    JuneAPI m_juneAPI;

    JuneInstance m_juneInstance{ nullptr };

private:
    std::function<void()> m_startCallback;
    std::function<void()> m_stopCallback;
};

} // namespace jipu