#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "Descriptor.hpp"

class DescriptorSet
{
public:
    std::vector<Descriptor> descriptors;
    VkDescriptorSet set;


    DescriptorSet(std::vector<Descriptor> descriptors);
};
