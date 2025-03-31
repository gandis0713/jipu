#include "june_sample.h"

#include <algorithm>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <unordered_set>

#include "hpc/counter.h"
#include "hpc/gpu.h"

namespace jipu
{

JuneSample::JuneSample(const SampleDescriptor& descriptor)
    : Window(descriptor.windowDescriptor)
    , m_appPath(descriptor.path)
    , m_appDir(descriptor.path.parent_path())
    , m_imgui(NativeImGui())
{
}

JuneSample::~JuneSample()
{
    if (m_imgui.has_value())
        m_imgui.value().clear();

    m_swapchain.reset();
    m_queue.reset();
    m_surface.reset();
    m_device.reset();
    m_physicalDevices.clear();
    m_adapter.reset();
    m_instance.reset();
}

void JuneSample::createInstance()
{
    InstanceDescriptor descriptor;
    m_instance = Instance::create(descriptor);
}

void JuneSample::createAdapter()
{
    AdapterDescriptor descriptor;
    descriptor.type = BackendAPI::kVulkan;
    m_adapter = m_instance->createAdapter(descriptor);
}

void JuneSample::getPhysicalDevices()
{
    m_physicalDevices = m_adapter->getPhysicalDevices();
}

void JuneSample::createSurface()
{
    SurfaceDescriptor descriptor;
    descriptor.windowHandle = getWindowHandle();
    m_surface = m_adapter->createSurface(descriptor);
}

void JuneSample::createSwapchain()
{
    if (m_surface == nullptr)
        throw std::runtime_error("Surface is null pointer.");

#if defined(__ANDROID__) || defined(ANDROID)
    TextureFormat textureFormat = TextureFormat::kRGBA8UnormSrgb;
#else
    TextureFormat textureFormat = TextureFormat::kBGRA8UnormSrgb;
#endif
    SwapchainDescriptor descriptor{
        .surface = m_surface.get(),
        .textureFormat = textureFormat,
        .presentMode = PresentMode::kFifo,
        .colorSpace = ColorSpace::kSRGBNonLinear,
        .width = m_width,
        .height = m_height,
        .queue = m_queue.get()
    };

    m_swapchain = m_device->createSwapchain(descriptor);
}

void JuneSample::createDevice()
{
    // TODO: select suit device.
    PhysicalDevice* physicalDevice = m_physicalDevices[0].get();

    DeviceDescriptor descriptor;
    m_device = physicalDevice->createDevice(descriptor);
}

void JuneSample::createQueue()
{
    QueueDescriptor descriptor{};

    m_queue = m_device->createQueue(descriptor);
}

void JuneSample::init()
{
    createInstance();
    createAdapter();
    getPhysicalDevices();
    createSurface();
    createDevice();
    createQueue();
    createSwapchain();

    if (m_imgui.has_value())
    {
        m_imgui.value().init(m_device.get(), m_queue.get(), m_swapchain.get());
    }

    Window::init();
}

void JuneSample::onUpdate()
{
    m_fps.update();
}

void JuneSample::onResize(uint32_t width, uint32_t height)
{
    if (m_swapchain)
        m_swapchain->resize(width, height);
}

void JuneSample::recordImGui(std::vector<std::function<void()>> cmds)
{
    if (m_imgui.has_value())
    {
        // set display size and mouse state.
        {
            ImGuiIO& io = ImGui::GetIO();
            io.DisplaySize = ImVec2((float)m_width, (float)m_height);
            io.MousePos = ImVec2(m_mouseX, m_mouseY);
            io.MouseDown[0] = m_leftMouseButton;
            io.MouseDown[1] = m_rightMouseButton;
            io.MouseDown[2] = m_middleMouseButton;
        }

        m_imgui.value().record(cmds);
        m_imgui.value().build();
    }
}

void JuneSample::windowImGui(const char* title, std::vector<std::function<void()>> uis)
{
    if (m_imgui.has_value())
    {
        m_imgui.value().window(title, uis);
    }
}

void JuneSample::drawImGui(CommandEncoder* commandEncoder, TextureView* renderView)
{
    if (m_imgui.has_value())
    {
        m_imgui.value().draw(commandEncoder, renderView);
    }
}

} // namespace jipu