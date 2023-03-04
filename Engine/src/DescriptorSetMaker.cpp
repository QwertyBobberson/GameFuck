#include "../include/DescriptorSetMaker.hpp"
#include "../include/Engine.hpp"

#include <iostream>
#include <stdexcept>

DescriptorSetMaker::DescriptorSetMaker()
{
    layoutBindings = std::vector<VkDescriptorSetLayoutBinding>();
}

void DescriptorSetMaker::CreateLayoutBinding(VkDescriptorType type, VkShaderStageFlags stages, int descriptorCount, VkSampler *immutableSamplers)
{
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = layoutBindings.size();
    layoutBinding.descriptorType = type;
    layoutBinding.descriptorCount = descriptorCount;
    layoutBinding.stageFlags = stages;
    layoutBinding.pImmutableSamplers = immutableSamplers;
    layoutBindings.push_back(layoutBinding);

    /*
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    */
}

void DescriptorSetMaker::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<unsigned int>(layoutBindings.size());
    layoutInfo.pBindings = layoutBindings.data();

    if(vkCreateDescriptorSetLayout(Engine::engine->device, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor set layout");
    }
}

void DescriptorSetMaker::CreateDescriptorPool(int maxSets)
{
    std::vector<VkDescriptorPoolSize> poolSizes{};
    for(size_t i = 0; i < layoutBindings.size(); i++)
    {
        poolSizes.push_back(VkDescriptorPoolSize());
        poolSizes[i].type = layoutBindings[i].descriptorType;
        poolSizes[i].descriptorCount = static_cast<unsigned int>(10);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<unsigned int>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<unsigned int>(maxSets);

    if(vkCreateDescriptorPool(Engine::engine->device, &poolInfo, nullptr, &pool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}


VkDescriptorSet DescriptorSetMaker::CreateDescriptorSet(std::vector<Descriptor> descriptors)
{
    std::cout << "Creating Descriptors {" << descriptors.size() << "}\n";
    std::vector<VkDescriptorSetLayout> layouts(Engine::engine->MaxFramesInFlight, layout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = layouts.data();

    VkDescriptorSet set;

    if(vkAllocateDescriptorSets(Engine::engine->device, &allocInfo, &set) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    std::vector<VkWriteDescriptorSet> descriptorWrites{};
    for(size_t j = 0; j < layoutBindings.size(); j++)
    {
        VkDescriptorType type = layoutBindings[j].descriptorType;

        descriptorWrites.push_back(VkWriteDescriptorSet());
        descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[j].dstSet = set;
        descriptorWrites[j].dstBinding = layoutBindings[j].binding;
        descriptorWrites[j].dstArrayElement = 0;
        descriptorWrites[j].descriptorType = type;
        descriptorWrites[j].descriptorCount = 1;

        if(descriptors[j].type == DescriptorType::image)
        {
            descriptorWrites[j].pImageInfo = (VkDescriptorImageInfo*)descriptors[j].info;
        }
        else if (descriptors[j].type == DescriptorType::buffer)
        {
            descriptorWrites[j].pBufferInfo = (VkDescriptorBufferInfo*)descriptors[j].info;
        }
        else
        {
            descriptorWrites[j].pTexelBufferView = (VkBufferView*)descriptors[j].info;
        }
    }

    vkUpdateDescriptorSets(Engine::engine->device, static_cast<unsigned int>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    return set;
}