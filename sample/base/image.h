#pragma once

#include <filesystem>

namespace vkt
{

class Image
{

public:
    Image(const std::filesystem::path& path);
    Image(void* buf, uint64_t len);
    ~Image();

    void* getPixels() const;
    int getWidth() const;
    int getHeight() const;
    int getChannel() const;

protected:
    void* m_pixels = nullptr;
    int m_width = 0;
    int m_height = 0;
    int m_channel = 0;
};

} // namespace vkt