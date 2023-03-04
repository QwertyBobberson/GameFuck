#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include "DescriptorSetMaker.hpp"


struct Pipeline
{
    VkPipeline pipeline;

    std::vector<VkPipelineShaderStageCreateInfo> shaders;
    VkPipelineVertexInputStateCreateInfo vertexInput;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkPipelineRasterizationStateCreateInfo rasterization;
    VkPipelineLayout pipelineLayout; //Still needs a function

    std::vector<DescriptorSetMaker> setMakers;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

    VkViewport viewport;
    VkRect2D scissor;

    Pipeline(std::string vertexShaderFile, std::string fragmentShaderFile);
    void CreatePipeline();
    void CreateCameraDescriptorSetLayout();
    void CreateObjectDescriptorSetLayout();
};

VkPipelineShaderStageCreateInfo CreatePipelineShaderInfo(VkShaderStageFlagBits stage, VkShaderModule shader);

VkPipelineInputAssemblyStateCreateInfo CreateInputCreateInfo(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

VkShaderModule CreateShaderModuleFromFile(std::string filename);

VkPipelineVertexInputStateCreateInfo CreateVertexInfo(std::vector<VkVertexInputBindingDescription> &bindingDescriptions, std::vector<VkVertexInputAttributeDescription> &attributeDescriptions);

VkVertexInputBindingDescription CreateBindingDescription(int size, int binding = 0);

VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo(VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL, float lineWidth = 1.0f, VkCullModeFlagBits cullMode = VK_CULL_MODE_NONE, VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE);

std::vector<VkVertexInputAttributeDescription> CreateAttributeDescriptions(std::vector<int> offsets, std::vector<int> sizes, int binding = 0);