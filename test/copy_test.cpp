#include "copy_test.h"

using namespace jipu;

TEST_F(CopyTest, test)
{
    const uint64_t uniformBufferSize = sizeof(float);

    BufferDescriptor uniformBufferDescriptor{};
    uniformBufferDescriptor.size = uniformBufferSize;
    uniformBufferDescriptor.usage = BufferUsageFlagBits::kUniform;
    auto uniformBuffer = m_device->createBuffer(uniformBufferDescriptor);
    EXPECT_NE(nullptr, uniformBuffer);
}
