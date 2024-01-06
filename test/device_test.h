#pragma once
#include "base/window_test.h"

namespace jipu
{

class Device;
class DeviceTest : public WindowTest
{
protected:
    void SetUp() override;
    void TearDown() override;
};

} // namespace jipu