#include "gtest/gtest.h"

#include "vk/instance.h"

#include <vector>

TEST(instance, test_) {
  std::vector<const char*> instance_extentions = {{"extension1"}, {"extension2"}};
  std::vector<const char*> layers = {{"layer1"}, {"layer2"}};

  vks::Instance instance(instance_extentions.data(), layers.data());
}