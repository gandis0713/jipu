#include "test_driver.h"
#include "gtest/gtest.h"

#include <spdlog/spdlog.h>

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    spdlog::set_level(spdlog::level::level_enum::debug);

    return RUN_ALL_TESTS();
}