#include "submit_test.h"

using namespace jipu;

void SubmitTest::SetUp()
{
    WindowTest::SetUp();
}
void SubmitTest::TearDown()
{
    WindowTest::TearDown();
}

TEST_F(SubmitTest, test)
{
    // dummy
    {
        BufferDescriptor bufferDescriptor{};
        bufferDescriptor.size = 1;
        bufferDescriptor.usage = BufferUsageFlagBits::kCopySrc;

        auto buffer = m_device->createBuffer(bufferDescriptor);
    }
}
