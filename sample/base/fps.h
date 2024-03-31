#include <chrono>

namespace jipu
{

class FPS
{
public:
    FPS();

public:
    void update();
    float fps();

private:
    std::chrono::milliseconds m_time = std::chrono::milliseconds::zero();
    uint64_t m_frame = 0;
    float m_fps = 0.0f;
};

} // namespace jipu
