#include "buffer_test.h"

#include "jipu/buffer.h"

using namespace jipu;

TEST_F(BufferTest, test_createbuffer_usage)
{
    std::unique_ptr<Buffer> buffer = nullptr;

    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = 1;

    {
        bufferDescriptor.usage = BufferUsageFlagBits::kMapRead;
        buffer = m_device->createBuffer(bufferDescriptor);
        ASSERT_NE(buffer, nullptr);
    }
    {
        bufferDescriptor.usage = BufferUsageFlagBits::kMapWrite;
        buffer = m_device->createBuffer(bufferDescriptor);
        ASSERT_NE(buffer, nullptr);
    }
    {
        bufferDescriptor.usage = BufferUsageFlagBits::kIndex;
        buffer = m_device->createBuffer(bufferDescriptor);
        ASSERT_NE(buffer, nullptr);
    }
    {
        bufferDescriptor.usage = BufferUsageFlagBits::kVertex;
        buffer = m_device->createBuffer(bufferDescriptor);
        ASSERT_NE(buffer, nullptr);
    }
    {
        bufferDescriptor.usage = BufferUsageFlagBits::kUniform;
        buffer = m_device->createBuffer(bufferDescriptor);
        ASSERT_NE(buffer, nullptr);
    }
    {
        bufferDescriptor.usage = BufferUsageFlagBits::kStorage;
        buffer = m_device->createBuffer(bufferDescriptor);
        ASSERT_NE(buffer, nullptr);
    }
    {
        bufferDescriptor.usage = BufferUsageFlagBits::kCopySrc;
        buffer = m_device->createBuffer(bufferDescriptor);
        ASSERT_NE(buffer, nullptr);
    }
    {
        bufferDescriptor.usage = BufferUsageFlagBits::kCopyDst;
        buffer = m_device->createBuffer(bufferDescriptor);
        ASSERT_NE(buffer, nullptr);
    }
}

TEST_F(BufferTest, test_createbuffer_with_0_size)
{
    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = 0;
    bufferDescriptor.usage = BufferUsageFlagBits::kVertex;
    ASSERT_ANY_THROW({ m_device->createBuffer(bufferDescriptor); });
}

TEST_F(BufferTest, test_createbuffer_with_max_size)
{
    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = std::numeric_limits<int64_t>::max();
    bufferDescriptor.usage = BufferUsageFlagBits::kVertex;
    ASSERT_ANY_THROW({ m_device->createBuffer(bufferDescriptor); });
}

TEST_F(BufferTest, test_createbuffer_with_1_size)
{
    BufferDescriptor bufferDescriptor{};
    bufferDescriptor.size = 1;
    bufferDescriptor.usage = BufferUsageFlagBits::kVertex;
    auto buffer = m_device->createBuffer(bufferDescriptor);
    ASSERT_NE(buffer, nullptr);
}
