#include "image.h"

#include <spdlog/spdlog.h>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace vkt
{

Image::Image(const std::filesystem::path& path)
{
    int components = 0;
    stbi_uc* pixels = stbi_load(path.string().c_str(), &m_width, &m_height, &components, STBI_rgb_alpha);

    if (pixels == nullptr)
    {
        throw std::runtime_error("failed to load texture image.");
    }

    m_pixels = pixels;
    m_channel = static_cast<int>(STBI_rgb_alpha);
}

Image::~Image()
{
    if (m_pixels != nullptr)
        stbi_image_free(m_pixels);
}

void* Image::getPixels() const
{
    return m_pixels;
}

int Image::getWidth() const
{
    return m_width;
}

int Image::getHeight() const
{
    return m_height;
}

int Image::getChannel() const
{
    return m_channel;
}

} // namespace vkt