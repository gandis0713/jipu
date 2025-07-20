#pragma once

#include <filesystem>
#include <vector>

namespace jipu
{

class Image
{

public:
    Image() = default;
    Image(const std::filesystem::path& path);
    Image(const std::filesystem::path& path, int targetWidth, int targetHeight, int targetChannels);
    Image(void* buf, uint64_t len);
    Image(void* buf, uint64_t len, int targetWidth, int targetHeight, int targetChannels);
    ~Image();

    void setPixels(unsigned char* pixels, int width, int height, int channels);
    unsigned char* getPixels();
    int getWidth() const;
    int getHeight() const;
    int getChannel() const;

    void save(const std::filesystem::path& path);

    void convert(int width, int height, int channels);

private:
    void convert(unsigned char* pixels, int width, int height, int channels, int targetWidth, int targetHeight, int targetChannels);

protected:
    std::vector<unsigned char> m_pixels{};
    int m_width = 0;
    int m_height = 0;
    int m_channel = 0;
};

} // namespace jipu