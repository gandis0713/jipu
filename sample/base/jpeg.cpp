#include "jpeg.h"

#include <spdlog/spdlog.h>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace vkt
{

JPEGImage::JPEGImage(const std::filesystem::path& path)
    : Image()
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

JPEGImage::~JPEGImage()
{
    if (m_pixels != nullptr)
        stbi_image_free(m_pixels);
}
} // namespace vkt