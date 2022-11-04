#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include "../include/Pipeline.hpp"
#include "../include/SwapChain.hpp"

#define PrintVariable(var) std::cout << #var << " " << var << std::endl;

VkPipelineShaderStageCreateInfo CreatePipelineShaderInfo(VkShaderStageFlagBits stage, VkShaderModule shader)
{
    return VkPipelineShaderStageCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        nullptr,
        NULL,
        stage,
        shader,
        "main",
        nullptr
    };
}

void CreatePipeline(Engine* engine, Pipeline &pipeline, SwapChain swapChain)
{
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    pipeline.scissor.offset = {0, 0};
    pipeline.scissor.extent = swapChain.extent;

    pipeline.viewport.x = 0.0f;
    pipeline.viewport.y = 0.0f;
    pipeline.viewport.width = (float)swapChain.extent.width;
    pipeline.viewport.height = (float)swapChain.extent.height;
    pipeline.viewport.minDepth = 0.0f;
    pipeline.viewport.maxDepth = 1.0f;

    viewportState.viewportCount = 1;
    viewportState.pViewports = &pipeline.viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &pipeline.scissor;


    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(engine->device, &pipelineLayoutInfo, nullptr, &pipeline.pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to Create pipeline layout!");
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f;
    depthStencil.maxDepthBounds = 1.0f;
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front = {};
    depthStencil.back = {};

    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pipelineInfo.stageCount = pipeline.shaders.size();
    pipelineInfo.pStages = pipeline.shaders.data();
    pipelineInfo.pVertexInputState = &pipeline.vertexInput;
    pipelineInfo.pInputAssemblyState = &pipeline.inputAssembly;

    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &pipeline.rasterization;


    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = pipeline.pipelineLayout;
    pipelineInfo.renderPass = pipeline.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VkResult result = vkCreateGraphicsPipelines(engine->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline);
    if(result != VK_SUCCESS)
    {
        std::cout << "Failed to create a pipeline, " << result << "\n";
    }
}

VkPipelineInputAssemblyStateCreateInfo CreateInputCreateInfo(VkPrimitiveTopology topology)
{
    return VkPipelineInputAssemblyStateCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        nullptr,
        NULL,
        topology,
        VK_FALSE
    };
}

VkShaderModule CreateShaderModuleFromFile(Engine* engine, std::string filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();


    VkShaderModuleCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    CreateInfo.codeSize = fileSize;
    CreateInfo.pCode = reinterpret_cast<const uint32_t *>(buffer.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(engine->device, &CreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to Create shader module!");
    }

    return shaderModule;
}

VkPipelineVertexInputStateCreateInfo CreateVertexInfo(std::vector<VkVertexInputBindingDescription> &bindingDescriptions, std::vector<VkVertexInputAttributeDescription> &attributeDescriptions)
{
    VkPipelineVertexInputStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
    createInfo.pVertexBindingDescriptions = bindingDescriptions.data();

    createInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    createInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    return createInfo;
}

VkVertexInputBindingDescription CreateBindingDescription(int size, int binding)
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = binding;
    bindingDescription.stride = size;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}


VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo(VkPolygonMode polygonMode, float lineWidth, VkCullModeFlagBits cullMode, VkFrontFace frontFace)
{
    VkPipelineRasterizationStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    createInfo.rasterizerDiscardEnable = VK_FALSE;
    createInfo.polygonMode = polygonMode;
    createInfo.lineWidth = lineWidth;
    createInfo.cullMode = cullMode;
    createInfo.frontFace = frontFace;
    createInfo.depthBiasEnable = false;

    return createInfo;
}

std::vector<VkVertexInputAttributeDescription> CreateAttributeDescriptions(std::vector<int> offsets, int binding)
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(offsets.size());
    for(size_t i = 0; i < offsets.size(); i++)
    {
        attributeDescriptions[i].binding = binding;
        attributeDescriptions[i].location = i;
        attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[i].offset = offsets[i];
    }

    return attributeDescriptions;
}
