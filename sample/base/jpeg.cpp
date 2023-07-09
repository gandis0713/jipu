#include "jpeg.h"

#include <spdlog/spdlog.h>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace vkt
{

JPEGImage::JPEGImage(const std::filesystem::path& path)
{
    const char* filename = reinterpret_cast<const char*>(path.c_str());
    stbi_uc* pixels = stbi_load(filename, &m_width, &m_height, &m_channels, STBI_rgb_alpha);

    if (pixels == nullptr)
    {
        throw std::runtime_error("failed to load texture image.");
    }

    m_pixels = pixels;
}

JPEGImage::~JPEGImage()
{
    if (m_pixels != nullptr)
        stbi_image_free(m_pixels);
}

void* JPEGImage::getPixels() const
{
    return m_pixels;
}

int JPEGImage::getWidth() const
{
    return m_width;
}

int JPEGImage::getHeight() const
{
    return m_height;
}

} // namespace vkt