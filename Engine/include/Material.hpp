#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Pipeline.hpp"
#include <vector>

class Material
{
    Pipeline* pipeline;
    VkPipelineLayout pipelineLayout;

    std::vector<VkDescriptorSet> descriptorSets;
};
