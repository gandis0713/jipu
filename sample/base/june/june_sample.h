#pragma once

#include "fps.h"
#include "hpc_watcher.h"
#include "june_service.h"
#include "native_imgui.h"
#include "window.h"

#include <deque>
#include <filesystem>
#include <optional>
#include <unordered_set>

#include <jipu/common/dylib.h>
#include <jipu/native/adapter.h>
#include <jipu/native/device.h>
#include <jipu/native/instance.h>
#include <jipu/native/physical_device.h>
#include <jipu/native/queue.h>
#include <jipu/native/surface.h>
#include <jipu/native/swapchain.h>

#include "june/june.h"
#include "june_api.h"

namespace jipu
{

struct JuneSampleDescriptor
{
    WindowDescriptor windowDescriptor;
    std::filesystem::path path;
};

class JuneSample : public Window
{
public:
    JuneSample() = delete;
    JuneSample(const JuneSampleDescriptor& descriptor);
    virtual ~JuneSample() = default;

protected:
    void loadJuneLibrary();
    void createInstance(const std::string& label);

protected:
    std::filesystem::path m_appPath;
    std::filesystem::path m_appDir;

    DyLib m_juneLib;
    JuneAPI m_juneAPI;
    JuneInstance m_juneInstance{ nullptr };
};

} // namespace jipu
