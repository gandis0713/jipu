#pragma once

#include "window.h"
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
    void performanceWindow();

private:
    struct FPS
    {
        std::chrono::milliseconds time = std::chrono::milliseconds::zero();
        uint64_t frame = 0;
        float fps = 0.0f;
    } m_fps;
    void updateFPS();
};

} // namespace jipu
