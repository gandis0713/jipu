#include "copy_test.h"

using namespace jipu;

void CopyTest::SetUp()
{
    WindowTest::SetUp();

    TextureDescriptor textureDescriptor{};
    textureDescriptor.type = TextureType::k2D;
    textureDescriptor.format = TextureFormat::kRGBA_8888_UInt_Norm;
    textureDescriptor.mipLevels = 1;
    textureDescriptor.sampleCount = 1;
    textureDescriptor.width = m_image.width;
    textureDescriptor.height = m_image.height;
    textureDescriptor.usage = TextureUsageFlagBits::kCopySrc |
                              TextureUsageFlagBits::kCopyDst |
                              TextureUsageFlagBits::kTextureBinding,

    m_imageTexture = m_device->createTexture(textureDescriptor);
}
void CopyTest::TearDown()
{
    m_imageBuffer.reset();
    m_imageTexture.reset();

    WindowTest::TearDown();
}

TEST_F(CopyTest, test_copyTextureToBuffer)
{
    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = m_image.width * m_image.height * m_image.channel * sizeof(char);
    bufferDescriptor.usage = BufferUsageFlagBits::kCopySrc;

    m_imageBuffer = m_device->createBuffer(bufferDescriptor);
    EXPECT_NE(nullptr, m_imageBuffer);
    void* imageBufferPointer = m_imageBuffer->map();
    EXPECT_NE(nullptr, imageBufferPointer);
    memcpy(imageBufferPointer, m_image.data.data(), bufferDescriptor.size);
    m_imageBuffer->unmap();

    BlitTextureBuffer blitTextureBuffer{};
    blitTextureBuffer.buffer = m_imageBuffer.get();
    blitTextureBuffer.bytesPerRow = m_image.width * m_image.channel * sizeof(char);
    blitTextureBuffer.rowsPerTexture = m_image.height;
    blitTextureBuffer.offset = 0;

    BlitTexture blitTexture{};
    blitTexture.texture = m_imageTexture.get();

    Extent3D extent{};
    extent.width = m_image.width;
    extent.height = m_image.height;
    extent.depth = 1;

    CommandBufferDescriptor commandBufferDescriptor{};
    commandBufferDescriptor.usage = CommandBufferUsage::kOneTime;

    auto commandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);

    CommandEncoderDescriptor commandEncoderDescriptor{};
    auto commandEncoder = commandBuffer->createCommandEncoder(commandEncoderDescriptor);

    commandEncoder->copyBufferToTexture(blitTextureBuffer, blitTexture, extent);
    commandEncoder->finish();

    QueueDescriptor queueDescriptor{};
    queueDescriptor.flags = QueueFlagBits::kTransfer;

    auto queue = m_device->createQueue(queueDescriptor);
    queue->submit({ commandBuffer.get() });
}
