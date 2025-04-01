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

#include <jipu/native/adapter.h>
#include <jipu/native/device.h>
#include <jipu/native/instance.h>
#include <jipu/native/physical_device.h>
#include <jipu/native/queue.h>
#include <jipu/native/surface.h>
#include <jipu/native/swapchain.h>

#include "hpc/instance.h"

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
    std::filesystem::path m_appPath;
    std::filesystem::path m_appDir;

    std::unique_ptr<JuneService> m_service1{ nullptr };
    std::unique_ptr<JuneService> m_service2{ nullptr };
    std::unique_ptr<JuneService> m_service3{ nullptr };
};

} // namespace jipu
