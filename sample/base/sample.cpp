#include "sample.h"

#include <algorithm>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

#include "hpc/counter.h"
#include "hpc/hpc.h"

namespace jipu
{

Sample::Sample(const SampleDescriptor& descriptor)
    : Window(descriptor.windowDescriptor)
    , m_appPath(descriptor.path)
    , m_appDir(descriptor.path.parent_path())
    , m_imgui(Im_Gui())
{
}

Sample::~Sample()
{
    if (m_hpcWatcher)
        m_hpcWatcher->stop();

    if (m_imgui.has_value())
        m_imgui.value().clear();

    m_queue.reset();
    m_swapchain.reset();
    m_surface.reset();
    m_device.reset();
    m_physicalDevices.clear();
    m_driver.reset();
}

void Sample::createDriver()
{
    DriverDescriptor descriptor;
    descriptor.type = DriverType::kVulkan;
    m_driver = Driver::create(descriptor);
}

void Sample::getPhysicalDevices()
{
    m_physicalDevices = m_driver->getPhysicalDevices();
}

void Sample::createSurface()
{
    SurfaceDescriptor descriptor;
    descriptor.windowHandle = getWindowHandle();
    m_surface = m_driver->createSurface(descriptor);
}

void Sample::createSwapchain()
{
    if (m_surface == nullptr)
        throw std::runtime_error("Surface is null pointer.");

#if defined(__ANDROID__) || defined(ANDROID)
    TextureFormat textureFormat = TextureFormat::kRGBA_8888_UInt_Norm_SRGB;
#else
    TextureFormat textureFormat = TextureFormat::kBGRA_8888_UInt_Norm_SRGB;
#endif
    SwapchainDescriptor descriptor{
        .surface = *m_surface,
        .textureFormat = textureFormat,
        .presentMode = PresentMode::kFifo,
        .colorSpace = ColorSpace::kSRGBNonLinear,
        .width = m_width,
        .height = m_height
    };

    m_swapchain = m_device->createSwapchain(descriptor);
}

void Sample::createDevice()
{
    // TODO: select suit device.
    PhysicalDevice* physicalDevice = m_physicalDevices[0].get();

    DeviceDescriptor descriptor;
    m_device = physicalDevice->createDevice(descriptor);
}

void Sample::createQueue()
{
    QueueDescriptor descriptor{};
    descriptor.flags = QueueFlagBits::kGraphics;

    m_queue = m_device->createQueue(descriptor);
}

void Sample::init()
{
    createDriver();
    getPhysicalDevices();
    createSurface();
    createDevice();
    createSwapchain();
    createQueue();

    if (m_imgui.has_value())
    {
        m_imgui.value().init(m_device.get(), m_queue.get(), *m_swapchain);
    }

    Window::init();
}

void Sample::update()
{
    m_fps.update();
    if (m_hpcWatcher)
    {
        m_hpcWatcher->update();
    }
}

void Sample::recordImGui(std::vector<std::function<void()>> cmds)
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

void Sample::windowImGui(const char* title, std::vector<std::function<void()>> uis)
{
    if (m_imgui.has_value())
    {
        m_imgui.value().window(title, uis);
    }
}

void Sample::drawImGui(CommandEncoder* commandEncoder, TextureView& renderView)
{
    if (m_imgui.has_value())
    {
        m_imgui.value().draw(commandEncoder, renderView);
    }
}

void Sample::onHPCListner(Values values)
{
    for (const auto& value : values)
    {
        switch (value.type)
        {
        case hpc::Sample::Type::uint64:
            m_profiling[value.counter].push_back(static_cast<float>(value.value.uint64));
            break;
        case hpc::Sample::Type::float64:
            m_profiling[value.counter].push_back(static_cast<float>(value.value.float64));
            break;
        }
    }
}

void Sample::createHPCWatcher(std::vector<hpc::Counter> counters)
{
    auto gpus = hpc::gpus();
    if (gpus.empty())
        return;

    // TODO: select gpu.
    auto gpu = gpus[0].get();

    std::vector<hpc::Counter> usableCounters{};
    const auto& availableHpcCounters = gpu->counters();
    for (const auto& counter : counters)
    {
        auto it = std::find(availableHpcCounters.begin(), availableHpcCounters.end(), counter);
        if (it != availableHpcCounters.end())
        {
            // insert empty values.
            m_profiling.insert({ counter, {} });
            usableCounters.push_back(counter);
        }
    }

    hpc::SamplerDescriptor descriptor{ .counters = usableCounters };
    hpc::Sampler::Ptr sampler = gpu->create(descriptor);

    HPCWatcherDescriptor watcherDescriptor{
        .sampler = std::move(sampler),
        .counters = counters,
        .listner = std::bind(&Sample::onHPCListner, this, std::placeholders::_1)
    };

    m_hpcWatcher = std::make_unique<HPCWatcher>(std::move(watcherDescriptor));
    m_hpcWatcher->start();
}

void Sample::profilingWindow()
{
    windowImGui(
        "Profiling", { [&]() {
            ImGui::Text("Common");
            ImGui::Separator();
            drawPolyline("FPS", m_fps.getAll());
            ImGui::Separator();

            ImGui::Text("GPU Profiling");
            ImGui::Separator();
            drawPolyline("Fragment Usage", m_profiling[hpc::Counter::FragmentUtilization], "%");
            drawPolyline("Non Fragment Usage", m_profiling[hpc::Counter::NonFragmentUtilization], "%");
            drawPolyline("Tiler Usage", m_profiling[hpc::Counter::TilerUtilization], "%");
            drawPolyline("External Read Bytes", m_profiling[hpc::Counter::ExternalReadBytes]);
            drawPolyline("External Write Bytes", m_profiling[hpc::Counter::ExternalWriteBytes]);
            drawPolyline("External Read Stall Rate", m_profiling[hpc::Counter::ExternalReadStallRate], "%");
            drawPolyline("External Write Stall Rate", m_profiling[hpc::Counter::ExternalWriteStallRate], "%");
            drawPolyline("External Read Latency 0", m_profiling[hpc::Counter::ExternalReadLatency0]);
            drawPolyline("External Read Latency 1", m_profiling[hpc::Counter::ExternalReadLatency1]);
            drawPolyline("External Read Latency 2", m_profiling[hpc::Counter::ExternalReadLatency2]);
            drawPolyline("External Read Latency 3", m_profiling[hpc::Counter::ExternalReadLatency3]);
            drawPolyline("External Read Latency 4", m_profiling[hpc::Counter::ExternalReadLatency4]);
            drawPolyline("External Read Latency 5", m_profiling[hpc::Counter::ExternalReadLatency5]);
            drawPolyline("Total Input Primitives", m_profiling[hpc::Counter::GeometryTotalInputPrimitives]);
            ImGui::Separator();
        } });
}

void Sample::drawPolyline(std::string title, std::deque<float> data, std::string unit)
{
    if (data.empty())
        return;

    const auto size = data.size();
    const std::string description = fmt::format("{:.1f} {}", data[data.size() - 1], unit.c_str());
    int offset = 0;
    if (size > 15)
        offset = size - 15;
    ImGui::PlotLines(title.c_str(), &data[offset], size - offset, 0, description.c_str());
}

} // namespace jipu