#pragma once

#include <filesystem>

namespace vkt
{

class JPEGImage final
{
public:
    JPEGImage(const std::filesystem::path& path);
    ~JPEGImage();

    void* getPixels() const;
    int getWidth() const;
    int getHeight() const;
    int getChannel() const;

private:
    void* m_pixels = nullptr;
    int m_width = 0;
    int m_height = 0;
};

} // namespace vkt