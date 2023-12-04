#pragma once
#include "base/window_test.h"

#include "jipu/texture.h"

class CopyTest : public WindowTest
{
protected:
    void SetUp() override;
    void TearDown() override;

protected:
    std::unique_ptr<jipu::Buffer> m_imageBuffer = nullptr;
    std::unique_ptr<jipu::Texture> m_imageTexture = nullptr;
    struct Image
    {
        Image()
            : width(256)
            , height(256)
            , channel(4)
            , data(width * height * channel, static_cast<char>(0xff))
        {
        }
        int width = 0;
        int height = 0;
        int channel = 0;
        std::vector<char> data{};
    } m_image;
};