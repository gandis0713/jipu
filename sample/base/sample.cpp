#include "sample.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <stdexcept>

namespace vkt
{

Sample::Sample(const SampleDescriptor& descriptor)
    : Window(descriptor.windowDescriptor)
    , m_appPath(descriptor.path)
    , m_appDir(descriptor.path.parent_path())
{
    ImGuiContext* imguiContext = ImGui::CreateContext();
    if (imguiContext == nullptr)
    {
        throw std::runtime_error("Failed to create imgui context");
    }

    ImGui_ImplSDL2_InitForVulkan(static_cast<SDL_Window*>(m_handle));

    auto scale = 10.0f;
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = scale;
    io.DisplaySize = ImVec2(m_width, m_height);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

    ImGui::Text("Hello, world %d", 123);
    // if (ImGui::Button("Save"))
    // MySaveFunction();
    // ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
    // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
}

Sample::~Sample()
{
    ImGui::DestroyContext();
}

} // namespace vkt