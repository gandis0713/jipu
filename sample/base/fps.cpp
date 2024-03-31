#include "fps.h"

using namespace std::chrono;

namespace jipu
{

FPS::FPS()
    : m_time(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()))
{
}

float FPS::fps()
{
    return m_fps;
}

void FPS::update()
{
    auto currentTime = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
    auto durationTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_time).count();

    ++m_frame;
    if (durationTime > 1000)
    {
        m_fps = m_frame * 1000.0 / durationTime;
        m_time = currentTime;
        m_frame = 0;
    }
}

} // namespace jipu
