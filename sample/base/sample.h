#pragma once

#include "fps.h"
#include "window.h"

#if defined(__ANDROID__) || defined(ANDROID)
#include "hwcpipe.h"
#endif

#include <deque>
#include <filesystem>
#include <optional>

#include <jipu/device.h>
#include <jipu/driver.h>
#include <jipu/physical_device.h>
#include <jipu/queue.h>
#include <jipu/surface.h>
#include <jipu/swapchain.h>

namespace jipu
{

struct SampleDescriptor
{
    WindowDescriptor windowDescriptor;
    std::filesystem::path path;
};

class Sample : public Window
{
public:
    Sample() = delete;
    Sample(const SampleDescriptor& descriptor);
    virtual ~Sample();

    void init() override;

    void recordImGui(std::vector<std::function<void()>> cmds);
    void windowImGui(const char* title, std::vector<std::function<void()>> uis);
    void drawImGui(CommandEncoder* commandEncoder, TextureView& renderView);

    virtual void createDriver();
    virtual void getPhysicalDevices();
    virtual void createSurface();
    virtual void createDevice();
    virtual void createSwapchain();
    virtual void createQueue();

protected:
    std::filesystem::path m_appPath;
    std::filesystem::path m_appDir;

    std::unique_ptr<Driver> m_driver = nullptr;
    std::vector<std::unique_ptr<PhysicalDevice>> m_physicalDevices{};
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Queue> m_queue = nullptr;

    std::optional<Im_Gui> m_imgui = std::nullopt;

protected:
    std::unique_ptr<CommandEncoder> m_commandEncoder = nullptr;
    TextureView* m_renderView = nullptr;

protected:
    void debuggingWindow();

private:
    FPS m_fps{};

#if defined(HWC_PIPE_ENABLED)
public:
    void configure(std::unordered_set<hwcpipe_counter> counters);

private:
    void createHWCPipe();

private:
    bool m_profiling = false;

private:
    HWCPipe m_hwcpipe{};
    std::optional<MaliGPU::Ref> m_maliGPU = std::nullopt;
#endif
};

} // namespace jipu
