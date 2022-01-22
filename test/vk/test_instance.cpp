#include "gtest/gtest.h"

#include "vk/instance.h"

#include <vector>

TEST(instance, test_) {
  const std::vector<const char*> instance_extentions = {{"extension1"}, {"extension2"}};
  const std::vector<const char*> layers = {{"layer1"}, {"layer2"}};

  vks::Instance instance(instance_extentions, layers);
}