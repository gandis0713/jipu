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

struct SampleDescriptor
{
    WindowDescriptor windowDescriptor;
    std::filesystem::path path;
};

class JuneSample : public Window
{
public:
    JuneSample() = delete;
    JuneSample(const SampleDescriptor& descriptor);
    virtual ~JuneSample();

public:
    virtual void createInstance();
    virtual void createAdapter();
    virtual void getPhysicalDevices();
    virtual void createSurface();
    virtual void createDevice();
    virtual void createSwapchain();
    virtual void createQueue();

public:
    void init() override;
    void onUpdate() override;
    void onResize(uint32_t width, uint32_t height) override;

public:
    void recordImGui(std::vector<std::function<void()>> cmds);
    void windowImGui(const char* title, std::vector<std::function<void()>> uis);
    void drawImGui(CommandEncoder* commandEncoder, TextureView* renderView);

protected:
    std::filesystem::path m_appPath;
    std::filesystem::path m_appDir;

    std::unique_ptr<Instance> m_instance = nullptr;
    std::unique_ptr<Adapter> m_adapter = nullptr;
    std::vector<std::unique_ptr<PhysicalDevice>> m_physicalDevices{};
    std::unique_ptr<Device> m_device = nullptr;
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Queue> m_queue = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;
    std::unique_ptr<CommandEncoder> m_commandEncoder = nullptr;
    TextureView* m_renderView = nullptr;

protected:
    std::optional<NativeImGui> m_imgui = std::nullopt;

private:
    FPS m_fps{};

    std::vector<std::unique_ptr<JuneService>> m_services{};
};

} // namespace jipu
