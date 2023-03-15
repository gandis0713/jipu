#include "platform.h"

namespace vkt
{

Platform::Platform(PlatformCreateInfo info) noexcept : m_adapter(info.adapter) {}

Platform::~Platform() noexcept {}

} // namespace vkt
