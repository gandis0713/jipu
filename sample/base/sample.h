#pragma once

#include "fps.h"
#include "hpc_watcher.h"
#include "window.h"

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

public:
    virtual void createDriver();
    virtual void getPhysicalDevices();
    virtual void createSurface();
    virtual void createDevice();
    virtual void createSwapchain();
    virtual void createQueue();

public:
    void init() override;
    void update() override;

public:
    void recordImGui(std::vector<std::function<void()>> cmds);
    void windowImGui(const char* title, std::vector<std::function<void()>> uis);
    void drawImGui(CommandEncoder* commandEncoder, TextureView& renderView);

public:
    void onHPCListner(std::unordered_map<Counter, double> values);

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

    std::unique_ptr<HPCWatcher> m_hpcWatcher = nullptr;

protected:
    std::unique_ptr<CommandEncoder> m_commandEncoder = nullptr;
    TextureView* m_renderView = nullptr;

protected:
    void createHPCWatcher(std::vector<Counter> counters);
    void debuggingWindow();

private:
    FPS m_fps{};
    bool m_profiling{ false };
};

} // namespace jipu
