#pragma once

#include <filesystem>

namespace jipu
{

class Image
{

public:
    Image(const std::filesystem::path& path);
    Image(const std::filesystem::path& path, int targetWidth, int targetHeight, int targetChannels);
    Image(void* buf, uint64_t len);
    Image(void* buf, uint64_t len, int targetWidth, int targetHeight, int targetChannels);
    ~Image();

    void* getPixels() const;
    int getWidth() const;
    int getHeight() const;
    int getChannel() const;

    void save(const std::filesystem::path& path);

protected:
    void* m_pixels = nullptr;
    int m_width = 0;
    int m_height = 0;
    int m_channel = 0;
};

} // namespace jipu