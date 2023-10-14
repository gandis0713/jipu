#include "sample.h"

#include <spdlog/spdlog.h>

#if defined(__ANDROID__) || defined(ANDROID)

    // GameActivity's C/C++ code
    #include <game-activity/GameActivity.cpp>
    #include <game-text-input/gametextinput.cpp>

// // Glue from GameActivity to android_main()
// // Passing GameActivity event from main thread to app native thread.
extern "C"
{
    #include <game-activity/native_app_glue/android_native_app_glue.c>
}

#endif

namespace vkt
{

class ImGuiSample : public Sample
{
public:
    ImGuiSample() = delete;
    ImGuiSample(const SampleDescriptor& descriptor);
    ~ImGuiSample() override;

public:
    void init() override;
    void draw() override;

private:
    void createDriver();
};

ImGuiSample::ImGuiSample(const SampleDescriptor& descriptor)
    : Sample(descriptor)
{
}

ImGuiSample::~ImGuiSample()
{
}

void ImGuiSample::init()
{
}

void ImGuiSample::draw()
{
}

} // namespace vkt

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    vkt::SampleDescriptor descriptor{
        { 800, 600, "Deferred Sample", nullptr },
        argv[0]
    };

    vkt::ImGuiSample sample(descriptor);

    return sample.exec();

    // ImGuiContext* imguiContext = ImGui::CreateContext();
    // if (imguiContext == nullptr)
    // {
    //     throw std::runtime_error("Failed to create imgui context");
    // }

    // auto scale = 10.0f;
    // ImGuiIO& io = ImGui::GetIO();
    // io.FontGlobalScale = scale;
    // // io.DisplaySize = ImVec2(m_width, m_height);
    // io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    // ImGuiStyle& style = ImGui::GetStyle();
    // style.ScaleAllSizes(scale);
    // style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
    // style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    // style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    // style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    // style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    // return 0;
}