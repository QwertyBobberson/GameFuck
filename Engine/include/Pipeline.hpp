#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include "Engine.hpp"

struct Pipeline
{
    VkPipeline pipeline;

    std::vector<VkPipelineShaderStageCreateInfo> shaders;
    VkPipelineVertexInputStateCreateInfo vertexInput;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkPipelineRasterizationStateCreateInfo rasterization;
    VkPipelineLayout pipelineLayout; //Still needs a function
    VkRenderPass renderPass;

    VkViewport viewport;
    VkRect2D scissor;
};

VkPipelineShaderStageCreateInfo CreatePipelineShaderInfo(VkShaderStageFlagBits stage, VkShaderModule shader);

void CreatePipeline(Engine* engine, Pipeline &pipeline, SwapChain swapChain);

VkPipelineInputAssemblyStateCreateInfo CreateInputCreateInfo(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

VkShaderModule CreateShaderModuleFromFile(Engine* engine, std::string filename);

VkPipelineVertexInputStateCreateInfo CreateVertexInfo(std::vector<VkVertexInputBindingDescription> &bindingDescriptions, std::vector<VkVertexInputAttributeDescription> &attributeDescriptions);

VkVertexInputBindingDescription CreateBindingDescription(int size, int binding = 0);

VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo(VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL, float lineWidth = 1.0f, VkCullModeFlagBits cullMode = VK_CULL_MODE_NONE, VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE);

std::vector<VkVertexInputAttributeDescription> CreateAttributeDescriptions(std::vector<int> offsets, int binding = 0);