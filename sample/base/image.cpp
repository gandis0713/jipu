#include "image.h"

#include <spdlog/spdlog.h>
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#include <vector>

namespace
{

std::vector<unsigned char> convertChannels(const unsigned char* input,
                                           int width, int height,
                                           int inputChannels,
                                           int outputChannels)
{
    int pixelCount = width * height;
    std::vector<unsigned char> output(pixelCount * outputChannels);

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
    int components = 0;
    stbi_uc* pixels = stbi_load(path.string().c_str(), &m_width, &m_height, &components, STBI_rgb_alpha);

    if (pixels == nullptr)
    {
        throw std::runtime_error("failed to load texture image by path.");
    }

    m_pixels = pixels;
    m_channel = static_cast<int>(STBI_rgb_alpha);
}

Image::Image(const std::filesystem::path& path, int targetWidth, int targetHeight, int targetChannels)
    : m_width(1)
    , m_height(1)
    , m_channel(1)
{
    int width, height, channels;
    stbi_uc* pixels = stbi_load(path.string().c_str(), &width, &height, nullptr, channels);

    if (pixels == nullptr)
    {
        throw std::runtime_error("failed to load texture image by path with specified dimensions.");
    }

    m_pixels = pixels;

    // 메모리 자동 관리를 위한 스마트 포인터
    std::unique_ptr<unsigned char[], decltype(&stbi_image_free)>
        dataPtr(pixels, &stbi_image_free);

    // 1. 채널 변환
    std::vector<unsigned char> channelConverted;
    int currentChannels = channels;

    if (targetChannels != channels)
    {
        channelConverted = convertChannels(pixels, width, height, channels, targetChannels);
        pixels = channelConverted.data();
        currentChannels = targetChannels;
    }

    // 2. 크기 변환
    std::vector<unsigned char> resizedData;
    int finalWidth = (targetWidth > 0) ? targetWidth : width;
    int finalHeight = (targetHeight > 0) ? targetHeight : height;

    if (finalWidth != width || finalHeight != height)
    {
        resizedData.resize(finalWidth * finalHeight * currentChannels);

        int result = stbir_resize_uint8(
            pixels, width, height, 0,
            resizedData.data(), finalWidth, finalHeight, 0,
            currentChannels);

        if (!result)
        {
            spdlog::error("Failed to resize image from {}x{} to {}x{} with {} channels",
                          width, height, finalWidth, finalHeight, currentChannels);
        }

        pixels = resizedData.data();
    }

    m_width = finalWidth;
    m_height = finalHeight;
    m_channel = currentChannels;
}

Image::Image(void* buf, uint64_t len)
{
    int components = 0;
    stbi_uc* pixels = stbi_load_from_memory(static_cast<const stbi_uc*>(buf), static_cast<int>(len), &m_width, &m_height, &components, STBI_rgb_alpha);

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

void Image::save(const std::filesystem::path& path)
{
    if (m_pixels == nullptr)
    {
        throw std::runtime_error("No image data to save.");
    }

    if (!stbi_write_png(path.string().c_str(), m_width, m_height, m_channel, m_pixels, m_width * m_channel))
    {
        throw std::runtime_error("Failed to save image to " + path.string());
    }

    spdlog::info("Image saved to {}", path.string());
}

} // namespace jipu