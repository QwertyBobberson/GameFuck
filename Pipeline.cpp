#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include "Engine.cpp"

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

void CreatePipeline(Engine* engine, Pipeline pipeline)
{
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    viewportState.viewportCount = 1;
    viewportState.pViewports = &pipeline.viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &pipeline.scissor;


    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlend{};
    colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.logicOpEnable = VK_FALSE;
    colorBlend.attachmentCount = 1;
    colorBlend.pAttachments = &colorBlendAttachment;

    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pipelineInfo.stageCount = pipeline.shaders.size();
    pipelineInfo.pStages = pipeline.shaders.data();
    pipelineInfo.pVertexInputState = &pipeline.vertexInput;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &pipeline.rasterization;
    pipelineInfo.pColorBlendState = &colorBlend;
    pipelineInfo.layout = pipeline.pipelineLayout;
    pipelineInfo.renderPass = pipeline.renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if(vkCreateGraphicsPipelines(engine->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline.pipeline) != VK_SUCCESS)
    {
        std::cout << "Failed to create a pipeline";
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
    CreateInfo.codeSize = buffer.size();
    CreateInfo.pCode = reinterpret_cast<const uint32_t *>(buffer.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(engine->device, &CreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to Create shader module!");
    }

    return shaderModule;
}

VkPipelineVertexInputStateCreateInfo CreateVertexInfo(std::vector<VkVertexInputBindingDescription> bindingDescriptions, std::vector<VkVertexInputAttributeDescription> attributeDescriptions)
{
    VkPipelineVertexInputStateCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    createInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
    createInfo.pVertexBindingDescriptions = bindingDescriptions.data();

    createInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    createInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

}

VkVertexInputBindingDescription CreateBindingDescription(int size, int binding = 0)
{
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = binding;
    bindingDescription.stride = size;
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}


VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo(VkPolygonMode polygonMode, float lineWidth = 1.0f, VkCullModeFlagBits cullMode = VK_CULL_MODE_NONE, VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE)
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

std::vector<VkVertexInputAttributeDescription> CreateAttributeDescriptions(std::vector<int> offsets, int binding = 0)
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    for(int i = 0; i < offsets.size(); i++)
    {
        attributeDescriptions[i].binding = binding;
        attributeDescriptions[i].location = i;
        attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[i].offset = offsets[i];
    }

    return attributeDescriptions;
}