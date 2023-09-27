#include "image.h"

#include <ktx.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace vkt
{

Image::Image(const std::filesystem::path& path)
{
    namespace fs = std::filesystem;

    fs::path ex = path.extension();
    if (ex == ".ktx")
    {
        ktxTexture* texture = nullptr;
        ktxResult ret = ktxTexture_CreateFromNamedFile((path).c_str(),
                                                       KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
                                                       &texture);
        if (ret != KTX_SUCCESS)
        {
            throw std::runtime_error("Failed to load KTX.");
        }

        m_width = texture->baseWidth;
        m_height = texture->baseHeight;
        m_pixels = ktxTexture_GetData(texture);
        m_channel = ktxTexture_GetElementSize(texture);
    }
    else // png, jpg and so on.
    {
        int components = 0;
        stbi_uc* pixels = stbi_load(path.string().c_str(), &m_width, &m_height, &components, STBI_rgb_alpha);

        if (pixels == nullptr)
        {
            throw std::runtime_error("failed to load texture image by path.");
        }

        m_pixels = pixels;
        m_channel = static_cast<int>(STBI_rgb_alpha);
    }
}

Image::Image(void* buf, uint64_t len)
{
    int components = 0;
    stbi_uc* pixels = stbi_load_from_memory(static_cast<const stbi_uc*>(buf), len, &m_width, &m_height, &components, STBI_rgb_alpha);

    if (pixels == nullptr)
    {
        throw std::runtime_error("Failed to load texture image by buffer.");
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