#pragma once

#include "jipu/common/dylib.h"
#include "june/june.h"
#include "june_api.h"
#include "runner.h"

namespace jipu
{

struct JuneServiceDescriptor
{
    uint32_t fps{ 0 };
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    void* windowHandle{ nullptr };
};

class JuneService
{
public:
    JuneService() = delete;
    JuneService(const JuneServiceDescriptor& descriptor);
    virtual ~JuneService();

public:
    void start();
    void stop();

public:
    virtual void begin();
    virtual void beforeWork();
    virtual void work();
    virtual void afterWork();
    virtual void end();

protected:
    void loadJuneLibrary();

protected:
    const JuneServiceDescriptor m_descriptor;

    Runner m_runner;

    DyLib m_juneLib;
    JuneAPI m_juneAPI;

    JuneInstance m_juneInstance{ nullptr };
};

} // namespace jipu