//
// Created by user on 2023/05/31.
//

#include "VKT.h"
#include <vulkan/vulkan.h>

VKT::VKT()
{
    std::cout << __func__ << std::endl;
}
VKT::~VKT()
{
    std::cout << __func__ << std::endl;
}

void VKT::print()
{
    std::cout << __func__ << std::endl;
}

std::string VKT::getName()
{
    return "VKT name";
}
