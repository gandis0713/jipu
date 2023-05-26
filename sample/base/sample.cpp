#include "sample.h"

std::filesystem::path Sample::path;
std::filesystem::path Sample::dir;

Sample::Sample(int argc, char** argv)
{
    path = std::filesystem::path(argv[0]);
    dir = path.parent_path();

    m_window = std::make_unique<Window>(800, 600, "vkt sample");
}

int Sample::exec()
{
    mainLoop();

    return 0;
}

void* Sample::getNativeWindow()
{
    return m_window->getNativeWindow();
}

void Sample::mainLoop()
{
    while (!m_window->shouldClose())
    {
        draw();
    }
}
