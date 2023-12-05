#include "copy_test.h"

using namespace jipu;

void CopyTest::SetUp()
{
    Test::SetUp();

    prepareStagingBuffer();
    prepareStagingTexture();
}
void CopyTest::TearDown()
{
    m_stagingBuffer.reset();
    m_stagingTexture.reset();

    Test::TearDown();
}

void CopyTest::prepareStagingBuffer()
{
    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = m_image.width * m_image.height * m_image.channel * sizeof(char);
    bufferDescriptor.usage = BufferUsageFlagBits::kCopySrc;

    m_stagingBuffer = m_device->createBuffer(bufferDescriptor);
    EXPECT_NE(nullptr, m_stagingBuffer);
    void* imageBufferPointer = m_stagingBuffer->map();
    EXPECT_NE(nullptr, imageBufferPointer);
    memcpy(imageBufferPointer, m_image.data.data(), bufferDescriptor.size);
    m_stagingBuffer->unmap();
}

void CopyTest::prepareStagingTexture()
{
    TextureDescriptor textureDescriptor{};
    textureDescriptor.type = TextureType::k2D;
    textureDescriptor.format = TextureFormat::kRGBA_8888_UInt_Norm;
    textureDescriptor.mipLevels = 1;
    textureDescriptor.sampleCount = 1;
    textureDescriptor.width = m_image.width;
    textureDescriptor.height = m_image.height;
    textureDescriptor.usage = TextureUsageFlagBits::kCopySrc;

    m_stagingTexture = m_device->createTexture(textureDescriptor);
    EXPECT_NE(nullptr, m_stagingTexture);
}

TEST_F(CopyTest, test_copyBufferToBuffer)
{
    CommandBufferDescriptor commandBufferDescriptor{};
    commandBufferDescriptor.usage = CommandBufferUsage::kOneTime;

    auto commandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);
    EXPECT_NE(nullptr, commandBuffer);

    CommandEncoderDescriptor commandEncoderDescriptor{};
    auto commandEncoder = commandBuffer->createCommandEncoder(commandEncoderDescriptor);
    EXPECT_NE(nullptr, commandEncoder);

    BlitBuffer srcBlitBuffer{};
    srcBlitBuffer.buffer = m_stagingBuffer.get();
    srcBlitBuffer.offset = 0;

    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = m_stagingBuffer->getSize();
    bufferDescriptor.usage = BufferUsageFlagBits::kCopyDst;

    auto dstBuffer = m_device->createBuffer(bufferDescriptor);
    EXPECT_NE(nullptr, dstBuffer);

    BlitBuffer dstBlitBuffer{};
    dstBlitBuffer.buffer = dstBuffer.get();
    dstBlitBuffer.offset = 0;

    commandEncoder->copyBufferToBuffer(srcBlitBuffer, dstBlitBuffer, m_stagingBuffer->getSize());
    EXPECT_EQ(commandBuffer.get(), commandEncoder->finish());

    QueueDescriptor queueDescriptor{};
    queueDescriptor.flags = QueueFlagBits::kTransfer;

    auto queue = m_device->createQueue(queueDescriptor);
    EXPECT_NE(nullptr, queue);
    queue->submit({ commandBuffer.get() });

    void* dstBufferPointer = dstBuffer->map();
    EXPECT_NE(nullptr, dstBufferPointer);

    char* dataPointer = static_cast<char*>(dstBufferPointer);
    char firstData = *dataPointer;
    EXPECT_EQ(firstData, m_image.data[0]);
}

TEST_F(CopyTest, test_copyBufferToTexture)
{
    BlitTextureBuffer blitTextureBuffer{};
    blitTextureBuffer.buffer = m_stagingBuffer.get();
    blitTextureBuffer.bytesPerRow = m_image.width * m_image.channel * sizeof(char);
    blitTextureBuffer.rowsPerTexture = m_image.height;
    blitTextureBuffer.offset = 0;

    TextureDescriptor textureDescriptor{};
    textureDescriptor.type = TextureType::k2D;
    textureDescriptor.format = TextureFormat::kRGBA_8888_UInt_Norm;
    textureDescriptor.mipLevels = 1;
    textureDescriptor.sampleCount = 1;
    textureDescriptor.width = m_image.width;
    textureDescriptor.height = m_image.height;
    textureDescriptor.usage = TextureUsageFlagBits::kCopySrc |
                              TextureUsageFlagBits::kCopyDst |
                              TextureUsageFlagBits::kTextureBinding;

    auto imageTexture = m_device->createTexture(textureDescriptor);
    EXPECT_NE(nullptr, imageTexture);

    BlitTexture blitTexture{};
    blitTexture.texture = imageTexture.get();

    Extent3D extent{};
    extent.width = m_image.width;
    extent.height = m_image.height;
    extent.depth = 1;

    CommandBufferDescriptor commandBufferDescriptor{};
    commandBufferDescriptor.usage = CommandBufferUsage::kOneTime;

    auto commandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);
    EXPECT_NE(nullptr, commandBuffer);

    CommandEncoderDescriptor commandEncoderDescriptor{};
    auto commandEncoder = commandBuffer->createCommandEncoder(commandEncoderDescriptor);
    EXPECT_NE(nullptr, commandEncoder);

    commandEncoder->copyBufferToTexture(blitTextureBuffer, blitTexture, extent);
    commandEncoder->finish();

    QueueDescriptor queueDescriptor{};
    queueDescriptor.flags = QueueFlagBits::kTransfer;

    auto queue = m_device->createQueue(queueDescriptor);
    EXPECT_NE(nullptr, queue);
    queue->submit({ commandBuffer.get() });
}

TEST_F(CopyTest, test_copyTextureToBuffer)
{
}

TEST_F(CopyTest, test_copyTextureToTexture)
{
}