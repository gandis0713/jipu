#pragma once
#include "base/test.h"

#include "jipu/texture.h"

namespace jipu
{

class CopyTest : public Test
{
protected:
    void SetUp() override;
    void TearDown() override;

    void prepareStagingBuffer();
    void prepareStagingTexture();

protected:
    std::unique_ptr<jipu::Buffer> m_stagingBuffer = nullptr;
    std::unique_ptr<jipu::Texture> m_stagingTexture = nullptr;
    struct Image
    {
        Image()
            : width(256)
            , height(256)
            , channel(4)
            , data(width * height * channel, static_cast<char>(0xf0))
        {
        }
        int width = 0;
        int height = 0;
        int channel = 0;
        std::vector<char> data{};
    } m_image;
};

} // namespace jipu