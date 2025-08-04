#include "image.h"

#include <spdlog/spdlog.h>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"
#include <vector>

namespace
{

stbir_pixel_layout getPixelLayout(int channels)
{
    switch (channels)
    {
    case 1:
        return STBIR_1CHANNEL;
    case 2:
        return STBIR_2CHANNEL;
    case 3:
        return STBIR_RGB;
    case 4:
        return STBIR_RGBA;
    default:
        return STBIR_RGBA;
    }
}

std::vector<unsigned char> convertChannels(const unsigned char* input,
                                           int width, int height,
                                           int inputChannels,
                                           int outputChannels)
{
    int pixelCount = width * height;
    std::vector<unsigned char> output(pixelCount * outputChannels);

    spdlog::info("Image Convert from inputChannels {} to outputChannels {}, pixelCount: {}", inputChannels, outputChannels, pixelCount);

    for (int i = 0; i < pixelCount; i++)
    {
        const unsigned char* srcPixel = input + i * inputChannels;
        unsigned char* dstPixel = output.data() + i * outputChannels;

        if (outputChannels == 1)
        { // 그레이스케일
            if (inputChannels >= 3)
            {
                // RGB를 그레이스케일로: 0.299*R + 0.587*G + 0.114*B
                dstPixel[0] = static_cast<unsigned char>(
                    0.299 * srcPixel[0] + 0.587 * srcPixel[1] + 0.114 * srcPixel[2]);
            }
            else
            {
                dstPixel[0] = srcPixel[0];
            }
        }
        else if (outputChannels == 3)
        { // RGB
            if (inputChannels == 1)
            {
                // 그레이스케일을 RGB로
                dstPixel[0] = dstPixel[1] = dstPixel[2] = srcPixel[0];
            }
            else if (inputChannels == 4)
            {
                // RGBA를 RGB로 (알파 채널 제거)
                dstPixel[0] = srcPixel[0];
                dstPixel[1] = srcPixel[1];
                dstPixel[2] = srcPixel[2];
            }
            else
            {
                memcpy(dstPixel, srcPixel, 3);
            }
        }
        else if (outputChannels == 4)
        { // RGBA
            if (inputChannels == 1)
            {
                // 그레이스케일을 RGBA로
                dstPixel[0] = dstPixel[1] = dstPixel[2] = srcPixel[0];
                dstPixel[3] = 255; // 완전 불투명
            }
            else if (inputChannels == 3)
            {
                // RGB를 RGBA로
                dstPixel[0] = srcPixel[0];
                dstPixel[1] = srcPixel[1];
                dstPixel[2] = srcPixel[2];
                dstPixel[3] = 255; // 완전 불투명
            }
            else
            {
                memcpy(dstPixel, srcPixel, 4);
            }
        }
    }

    return output;
}
} // namespace

namespace jipu
{

Image::Image(const std::filesystem::path& path)
{
    int channels = 0;
    int requiredChannels = static_cast<int>(STBI_rgb_alpha); // Always load as RGBA
    stbi_uc* pixels = stbi_load(path.string().c_str(), &m_width, &m_height, &channels, requiredChannels);

    size_t size = m_width * m_height * requiredChannels;
    m_pixels.clear();
    m_pixels.resize(size);
    memcpy(m_pixels.data(), pixels, size);
    stbi_image_free(pixels);

    if (m_pixels.empty())
    {
        throw std::runtime_error("failed to load texture image by path.");
    }

    if (channels != requiredChannels)
    {
        spdlog::warn("Loaded image has {} channels, expected {}", channels, requiredChannels);
    }
    m_channel = requiredChannels;
}

Image::Image(const std::filesystem::path& path, int targetWidth, int targetHeight, int targetChannels)
    : m_width(1)
    , m_height(1)
    , m_channel(1)
{
    int width, height, channels;
    int requiredChannels = static_cast<int>(STBI_rgb_alpha); // Always load as RGBA
    stbi_uc* pixels = stbi_load(path.string().c_str(), &width, &height, &channels, requiredChannels);

    if (pixels == nullptr)
    {
        throw std::runtime_error("failed to load texture image by path with specified dimensions.");
    }

    if (channels != requiredChannels)
    {
        spdlog::warn("Loaded image has {} channels, expected {}", channels, requiredChannels);
    }

    convert(pixels, width, height, requiredChannels, targetWidth, targetHeight, targetChannels);
}

Image::Image(void* buf, uint64_t len)
{
    int channels = 0;
    int requiredChannels = static_cast<int>(STBI_rgb_alpha); // Always load as RGBA
    stbi_uc* pixels = stbi_load_from_memory(static_cast<const stbi_uc*>(buf), static_cast<int>(len), &m_width, &m_height, &channels, requiredChannels);

    size_t size = m_width * m_height * requiredChannels;
    m_pixels.clear();
    m_pixels.resize(size);
    memcpy(m_pixels.data(), pixels, size);
    stbi_image_free(pixels);

    if (m_pixels.empty())
    {
        throw std::runtime_error("failed to load texture image by path.");
    }

    if (channels != requiredChannels)
    {
        spdlog::warn("Loaded image has {} channels, expected {}", channels, requiredChannels);
    }

    m_channel = requiredChannels;
}

Image::Image(void* buf, uint64_t len, int targetWidth, int targetHeight, int targetChannels)
    : m_width(1)
    , m_height(1)
    , m_channel(1)
{
    int width, height, channels;
    int requiredChannels = static_cast<int>(STBI_rgb_alpha); // Always load as RGBA
    stbi_uc* pixels = stbi_load_from_memory(static_cast<const stbi_uc*>(buf), static_cast<int>(len), &width, &height, &channels, requiredChannels);

    if (pixels == nullptr)
    {
        throw std::runtime_error("Failed to load texture image by buffer with specified dimensions.");
    }

    if (channels != requiredChannels)
    {
        spdlog::warn("Loaded image has {} channels, expected {}", channels, requiredChannels);
    }

    convert(pixels, width, height, requiredChannels, targetWidth, targetHeight, targetChannels);
}

Image::~Image()
{
    m_pixels.clear();
}

void Image::setPixels(unsigned char* pixels, int width, int height, int channels)
{
    m_width = width;
    m_height = height;
    m_channel = channels;

    size_t size = width * height * channels;
    if (size == 0)
    {
        throw std::runtime_error("Failed to allocate memory for image pixels.");
    }

    m_pixels.clear();
    m_pixels.resize(size);

    memcpy(m_pixels.data(), pixels, size);
}

unsigned char* Image::getPixels()
{
    return m_pixels.data();
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

void Image::save(const std::filesystem::path& path)
{
    if (m_pixels.empty())
    {
        throw std::runtime_error("No image data to save.");
    }

    if (!stbi_write_png(path.string().c_str(), m_width, m_height, m_channel, m_pixels.data(), m_width * m_channel))
    {
        throw std::runtime_error("Failed to save image to " + path.string());
    }

    spdlog::info("Image saved to {}", path.string());
}

void Image::convert(int width, int height, int channels)
{
    if (width <= 0 || height <= 0 || channels <= 0)
    {
        throw std::invalid_argument("Invalid target dimensions or channels for image conversion.");
    }

    std::vector<unsigned char> pixels = m_pixels;

    convert(pixels.data(), m_width, m_height, m_channel, width, height, channels);
}

void Image::convert(unsigned char* pixels, int width, int height, int channels, int targetWidth, int targetHeight, int targetChannels)
{
    // 1. Convert channels
    std::vector<unsigned char> channelConverted;
    int currentChannels = channels;

    if (targetChannels != channels)
    {
        channelConverted = convertChannels(pixels, width, height, channels, targetChannels);
        pixels = channelConverted.data();
        currentChannels = targetChannels;
    }

    // 2. Resize
    int finalWidth = (targetWidth > 0) ? targetWidth : width;
    int finalHeight = (targetHeight > 0) ? targetHeight : height;

    if (finalWidth != width || finalHeight != height || currentChannels != channels)
    {
        size_t resizedSize = finalWidth * finalHeight * currentChannels;
        m_pixels.clear();
        m_pixels.resize(resizedSize);

        // get pixel layout for stb_image_resize2
        stbir_pixel_layout pixelLayout = getPixelLayout(currentChannels);

        // High-quality resizing using stb_image_resize2
        void* result = stbir_resize(
            pixels, width, height, width * currentChannels,
            m_pixels.data(), finalWidth, finalHeight, finalWidth * currentChannels,
            pixelLayout, STBIR_TYPE_UINT8, STBIR_EDGE_CLAMP, STBIR_FILTER_MITCHELL);

        if (!result)
        {
            throw std::runtime_error("Failed to resize image.");
        }
    }

    m_width = finalWidth;
    m_height = finalHeight;
    m_channel = currentChannels;
}

} // namespace jipu