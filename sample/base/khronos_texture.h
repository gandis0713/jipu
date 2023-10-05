#pragma once

#include <filesystem>

#include <ktx.h>

namespace vkt
{

class KTX final
{

public:
    KTX(const std::filesystem::path& path);
    KTX(void* buf, uint64_t len);
    ~KTX();

    void* getPixels() const;
    int getWidth() const;
    int getHeight() const;
    int getChannel() const;

private:
    ktxTexture* m_texture = nullptr;
};

} // namespace vkt