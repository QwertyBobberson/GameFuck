#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include "DescriptorSet.hpp"
#include "Engine.hpp"

class DescriptorSetMaker
{
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
public:
    VkDescriptorSetLayout layout;
    VkDescriptorPool pool;
    DescriptorSetMaker();

    void CreateLayoutBinding(VkDescriptorType type, VkShaderStageFlags stages, int descriptorCount = 1, VkSampler *immutableSamplers = nullptr);
    void CreateDescriptorSetLayout();
    void CreateDescriptorPool(int maxSets);
    VkDescriptorSet CreateDescriptorSet(std::vector<Descriptor> descriptors);
};
