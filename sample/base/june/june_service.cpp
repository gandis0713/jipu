#include "june_service.h"

#include "june/june_sample.h"

#include <spdlog/spdlog.h>

namespace jipu
{
JuneService::JuneService(const JuneServiceDescriptor& descriptor)
    : m_descriptor(descriptor)
    , m_runner(m_descriptor.fps)
{
    loadJuneLibrary();
}

JuneService::~JuneService()
{
    stop();

    if (m_juneLib.isValid())
        m_juneLib.close();
}

void JuneService::start(const JuneServiceStartDescriptor& descriptor)
{
    m_startCallback = descriptor.callback;
    start();
}

void JuneService::pause()
{
    if (m_runner.isRunning())
        m_runner.pause();
}

void JuneService::resume()
{
    if (m_runner.isRunning())
        m_runner.resume();
}

void JuneService::stop(const JuneServiceStopDescriptor& descriptor)
{
    m_stopCallback = descriptor.callback;

    stop();

    if (m_stopCallback)
        m_stopCallback();
}

void JuneService::addSharedMemory(JuneSharedMemory sharedMemory)
{
    std::lock_guard<std::mutex> lock(m_sharedMemoryMutex);

    m_sharedMemories.push_back(sharedMemory);
}

JuneFence JuneService::getSignalFence() const
{
    return m_signalFence;
}

void JuneService::connectMemoryNode(JuneMemoryNode* inputNode)
{
    std::lock_guard<std::mutex> lock(m_memoryNodeMutex);
    m_memoryNode->connect(inputNode);
}

void JuneService::disconnectMemoryNode(JuneMemoryNode* inputNode)
{
    std::lock_guard<std::mutex> lock(m_memoryNodeMutex);
    m_memoryNode->disconnect(inputNode);
}

void JuneService::addWaitFence(JuneFence fence)
{
    std::lock_guard<std::mutex> lock(m_waitFenceMutex);

    m_waitFences.push_back(fence);
}

std::vector<JuneFence> JuneService::getWaitFences() const
{
    std::lock_guard<std::mutex> lock(m_waitFenceMutex);

    return m_waitFences;
}

std::vector<JuneSharedMemory> JuneService::getSharedMemories() const
{
    std::lock_guard<std::mutex> lock(m_sharedMemoryMutex);

    return m_sharedMemories;
}

JuneMemoryNode* JuneService::getMemoryNode() const
{
    std::lock_guard<std::mutex> lock(m_memoryNodeMutex);
    return m_memoryNode.get();
}

void JuneService::begin()
{
}

void JuneService::beforeWork()
{
    // Prepare for work, e.g., set up resources
}

void JuneService::work()
{
    // Main work loop, e.g., rendering or processing
}

void JuneService::afterWork()
{
    // Clean up after work, e.g., release resources
}

void JuneService::end()
{
}

void JuneService::start()
{
    m_runner.run(
        [this]() {
            begin();
            if (m_startCallback)
                m_startCallback();
        },
        [this]() {
            beforeWork();
            work();
            afterWork();
        },
        [this]() {
            end();
        });
}

void JuneService::stop()
{
    if (m_runner.isRunning())
        m_runner.stop();
}

void JuneService::addBeforeWork(const std::function<void()>& work)
{
    std::lock_guard<std::mutex> lock(m_beforeWorkMutex);
    m_beforeWorks.push(work);
}

void JuneService::addAfterWork(const std::function<void()>& work)
{
    std::lock_guard<std::mutex> lock(m_afterWorkMutex);
    m_afterWorks.push(work);
}

void JuneService::loadJuneLibrary()
{
    std::string juneLibName;
#if defined(__ANDROID__) || defined(ANDROID)
    juneLibName = "libjune.so";
#elif defined(__linux__)
    juneLibName = "libjune.so";
#elif defined(__APPLE__)
    juneLibName = "libjune.dylib";
#elif defined(WIN32)
    juneLibName = "june.dll";
#endif
    if (!m_juneLib.open(juneLibName.c_str()))
    {
        throw std::runtime_error("Failed to open library");
    }
    if (!m_juneAPI.loadProcs(&m_juneLib))
    {
        throw std::runtime_error("Failed to load procs");
    }
}

void JuneService::createInstance(const std::string& label)
{
    JuneInstanceDescriptor juneInstanceDescriptor{};
    juneInstanceDescriptor.label.data = label.data();
    juneInstanceDescriptor.label.length = label.length();

    m_juneInstance = m_juneAPI.CreateInstance(&juneInstanceDescriptor);
    if (!m_juneInstance)
    {
        spdlog::error("Failed to create June instance with label: {}", label);
        return;
    }
}

void JuneService::recordImGui(std::vector<std::function<void()>> cmds)
{
    if (m_imgui.has_value())
    {
        JuneServiceSharingData* sharingData = m_descriptor.sharingData;
        // set display size and mouse state.
        {
            ImGuiIO& io = ImGui::GetIO();
            io.DisplaySize = ImVec2((float)sharingData->width, (float)sharingData->height);
            io.MousePos = ImVec2(sharingData->mouseX, sharingData->mouseY);
            io.MouseDown[0] = sharingData->leftMouseButton;
            io.MouseDown[1] = sharingData->rightMouseButton;
            io.MouseDown[2] = sharingData->middleMouseButton;
        }

        m_imgui.value().record(cmds);
        m_imgui.value().build();
    }
}

void JuneService::windowImGui(const char* title, std::vector<std::function<void()>> uis)
{
    if (m_imgui.has_value())
    {
        m_imgui.value().window(title, uis);
    }
}

void JuneService::drawImGui(CommandEncoder* commandEncoder, TextureView* renderView)
{
    if (m_imgui.has_value())
    {
        m_imgui.value().draw(commandEncoder, renderView);
    }
}

} // namespace jipu