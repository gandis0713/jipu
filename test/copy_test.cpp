#include "copy_test.h"

using namespace jipu;

void CopyTest::SetUp()
{
    Test::SetUp();
}
void CopyTest::TearDown()
{
    Test::TearDown();
}

TEST_F(CopyTest, testCopy)
{
    // prepare source buffer
    std::unique_ptr<Buffer> srcBuffer = nullptr;
    {
        BufferDescriptor bufferDescriptor{};
        bufferDescriptor.size = m_image.width * m_image.height * m_image.channel * sizeof(char);
        bufferDescriptor.usage = BufferUsageFlagBits::kCopySrc;

        srcBuffer = m_device->createBuffer(bufferDescriptor);
        EXPECT_NE(nullptr, srcBuffer);
        void* imageBufferPointer = srcBuffer->map();
        EXPECT_NE(nullptr, imageBufferPointer);
        memcpy(imageBufferPointer, m_image.data.data(), bufferDescriptor.size);
        srcBuffer->unmap();
    }

    // copy buffer to buffer
    std::unique_ptr<Buffer> buffer = nullptr;
    {
        CommandBufferDescriptor commandBufferDescriptor{};
        commandBufferDescriptor.usage = CommandBufferUsage::kOneTime;

        auto commandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);
        EXPECT_NE(nullptr, commandBuffer);

        CommandEncoderDescriptor commandEncoderDescriptor{};
        auto commandEncoder = commandBuffer->createCommandEncoder(commandEncoderDescriptor);
        EXPECT_NE(nullptr, commandEncoder);

        BlitBuffer srcBlitBuffer{};
        srcBlitBuffer.buffer = srcBuffer.get();
        srcBlitBuffer.offset = 0;

        BufferDescriptor bufferDescriptor{};
        bufferDescriptor.size = srcBuffer->getSize();
        bufferDescriptor.usage = BufferUsageFlagBits::kCopyDst;

        buffer = m_device->createBuffer(bufferDescriptor);
        EXPECT_NE(nullptr, buffer);

        BlitBuffer dstBlitBuffer{};
        dstBlitBuffer.buffer = buffer.get();
        dstBlitBuffer.offset = 0;

        commandEncoder->copyBufferToBuffer(srcBlitBuffer, dstBlitBuffer, srcBuffer->getSize());
        EXPECT_EQ(commandBuffer.get(), commandEncoder->finish());

        QueueDescriptor queueDescriptor{};
        queueDescriptor.flags = QueueFlagBits::kTransfer;

        auto queue = m_device->createQueue(queueDescriptor);
        EXPECT_NE(nullptr, queue);
        queue->submit({ commandBuffer.get() });

        void* dstBufferPointer = buffer->map();
        EXPECT_NE(nullptr, dstBufferPointer);

        char* dataPointer = static_cast<char*>(dstBufferPointer);
        char firstData = *dataPointer;
        EXPECT_EQ(firstData, m_image.data[0]);
    }

    // copy buffer to texture
    std::unique_ptr<Texture> texture = nullptr;
    {
        TextureDescriptor textureDescriptor{};
        textureDescriptor.type = TextureType::k2D;
        textureDescriptor.format = TextureFormat::kRGBA_8888_UInt_Norm;
        textureDescriptor.mipLevels = 1;
        textureDescriptor.sampleCount = 1;
        textureDescriptor.width = m_image.width;
        textureDescriptor.height = m_image.height;
        textureDescriptor.usage = TextureUsageFlagBits::kCopyDst;

        texture = m_device->createTexture(textureDescriptor);
        EXPECT_NE(nullptr, texture);

        BlitTextureBuffer blitTextureBuffer{};
        blitTextureBuffer.buffer = srcBuffer.get();
        blitTextureBuffer.bytesPerRow = m_image.width * m_image.channel * sizeof(char);
        blitTextureBuffer.rowsPerTexture = m_image.height;
        blitTextureBuffer.offset = 0;

        BlitTexture blitTexture{};
        blitTexture.texture = texture.get();
        blitTexture.aspect = TextureAspectFlagBits::kColor;

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

    // prepare source texture
    std::unique_ptr<Texture> srcTexture = nullptr;
    {
        TextureDescriptor textureDescriptor{};
        textureDescriptor.type = TextureType::k2D;
        textureDescriptor.format = TextureFormat::kRGBA_8888_UInt_Norm;
        textureDescriptor.mipLevels = 1;
        textureDescriptor.sampleCount = 1;
        textureDescriptor.width = m_image.width;
        textureDescriptor.height = m_image.height;
        textureDescriptor.usage = TextureUsageFlagBits::kCopySrc | TextureUsageFlagBits::kCopyDst;

        srcTexture = m_device->createTexture(textureDescriptor);
        EXPECT_NE(nullptr, srcTexture);

        BlitTextureBuffer blitTextureBuffer{};
        blitTextureBuffer.buffer = srcBuffer.get();
        blitTextureBuffer.bytesPerRow = m_image.width * m_image.channel * sizeof(char);
        blitTextureBuffer.rowsPerTexture = m_image.height;
        blitTextureBuffer.offset = 0;

        BlitTexture blitTexture{};
        blitTexture.texture = srcTexture.get();
        blitTexture.aspect = TextureAspectFlagBits::kColor;

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

    // copy texture to buffer
    {
        BufferDescriptor dstBufferDescriptor{};
        dstBufferDescriptor.size = srcBuffer->getSize(); // TODO: from texture
        dstBufferDescriptor.usage = BufferUsageFlagBits::kCopyDst;

        auto dstBuffer = m_device->createBuffer(dstBufferDescriptor);
        EXPECT_NE(nullptr, dstBuffer);

        BlitTextureBuffer dstBlitBuffer{};
        dstBlitBuffer.buffer = dstBuffer.get();
        dstBlitBuffer.bytesPerRow = m_image.width * m_image.channel * sizeof(char);
        dstBlitBuffer.rowsPerTexture = m_image.height;
        dstBlitBuffer.offset = 0;

        BlitTexture srcBlitTexture{};
        srcBlitTexture.texture = srcTexture.get();
        srcBlitTexture.aspect = TextureAspectFlagBits::kColor;

        Extent3D extent{};
        extent.depth = 1;
        extent.width = srcTexture->getWidth();
        extent.height = srcTexture->getHeight();

        CommandBufferDescriptor commandBufferDescriptor{};
        commandBufferDescriptor.usage = CommandBufferUsage::kOneTime;

        auto commandBuffer = m_device->createCommandBuffer(commandBufferDescriptor);
        EXPECT_NE(nullptr, commandBuffer);

        CommandEncoderDescriptor commandEncoderDescriptor{};
        auto commandEncoder = commandBuffer->createCommandEncoder(commandEncoderDescriptor);
        EXPECT_NE(nullptr, commandEncoder);

        commandEncoder->copyTextureToBuffer(srcBlitTexture, dstBlitBuffer, extent);
        commandEncoder->finish();

        QueueDescriptor queueDescriptor{};
        queueDescriptor.flags = QueueFlagBits::kTransfer;

        auto queue = m_device->createQueue(queueDescriptor);
        EXPECT_NE(nullptr, queue);
        queue->submit({ commandBuffer.get() });
    }

    // copy texture to texture
    {
    }
}