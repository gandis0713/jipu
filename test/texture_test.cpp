#include "texture_test.h"

#include "jipu/texture.h"

using namespace jipu;

TEST_F(TextureTest, test_createtexture_usage)
{
    std::unique_ptr<Texture> texture = nullptr;

    TextureDescriptor descriptor{};
    descriptor.width = 1;
    descriptor.height = 1;
    descriptor.depth = 1;
    descriptor.mipLevels = 1;
    descriptor.sampleCount = 1;
    descriptor.type = TextureType::k2D;
    descriptor.format = TextureFormat::kRGBA_8888_UInt_Norm;

    {
        descriptor.usage = TextureUsageFlagBits::kUndefined;
        ASSERT_ANY_THROW({ m_device->createTexture(descriptor); });
    }

    {
        descriptor.usage = TextureUsageFlagBits::kColorAttachment;
        texture = m_device->createTexture(descriptor);
        ASSERT_NE(texture, nullptr);
    }

    {
        descriptor.usage = TextureUsageFlagBits::kCopyDst;
        texture = m_device->createTexture(descriptor);
        ASSERT_NE(texture, nullptr);
    }

    {
        descriptor.usage = TextureUsageFlagBits::kCopySrc;
        texture = m_device->createTexture(descriptor);
        ASSERT_NE(texture, nullptr);
    }

    {
        descriptor.usage = TextureUsageFlagBits::kDepthStencil;
        texture = m_device->createTexture(descriptor);
        ASSERT_NE(texture, nullptr);
    }

    {
        descriptor.usage = TextureUsageFlagBits::kStorageBinding;
        texture = m_device->createTexture(descriptor);
        ASSERT_NE(texture, nullptr);
    }

    {
        descriptor.usage = TextureUsageFlagBits::kTextureBinding;
        texture = m_device->createTexture(descriptor);
        ASSERT_NE(texture, nullptr);
    }
}

TEST_F(TextureTest, test_createtexture_with_size)
{
    std::unique_ptr<Texture> texture = nullptr;

    TextureDescriptor descriptor{};
    descriptor.mipLevels = 1;
    descriptor.sampleCount = 1;
    descriptor.type = TextureType::k2D;
    descriptor.format = TextureFormat::kRGBA_8888_UInt_Norm;
    descriptor.usage = TextureUsageFlagBits::kTextureBinding;

    {
        descriptor.width = 0;
        descriptor.height = 1;
        descriptor.depth = 1;
        ASSERT_ANY_THROW({ m_device->createTexture(descriptor); });
    }

    {
        descriptor.width = 1;
        descriptor.height = 0;
        descriptor.depth = 1;
        ASSERT_ANY_THROW({ m_device->createTexture(descriptor); });
    }

    {
        descriptor.width = 1;
        descriptor.height = 1;
        descriptor.depth = 0;
        ASSERT_ANY_THROW({ m_device->createTexture(descriptor); });
    }
}
