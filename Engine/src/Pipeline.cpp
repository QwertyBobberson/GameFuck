#include "../include/Pipeline.hpp"

Pipeline::Pipeline(std::string vertexShaderFile, std::string fragmentShaderFile)
{
	rasterization = CreateRasterizationStateCreateInfo();
    inputAssembly = CreateInputCreateInfo();

    std::vector<VkVertexInputBindingDescription> binding = {CreateBindingDescription(sizeof(Vertex))};
	std::vector<VkVertexInputAttributeDescription> attributes = CreateAttributeDescriptions({offsetof(Vertex, pos), offsetof(Vertex, col), offsetof(Vertex, uv), offsetof(Vertex, normal)}, {sizeof(glm::vec3), sizeof(glm::vec3), sizeof(glm::vec2), sizeof(glm::vec3)});
	vertexInput = CreateVertexInfo({binding}, attributes);

	VkShaderModule vertShader = CreateShaderModuleFromFile(vertexShaderFile);
	VkShaderModule fragShader = CreateShaderModuleFromFile(fragmentShaderFile);

	shaders = {CreatePipelineShaderInfo(VK_SHADER_STAGE_VERTEX_BIT, vertShader), CreatePipelineShaderInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragShader)};

    CreateCameraDescriptorSetLayout();
    CreateObjectDescriptorSetLayout();
	CreatePipeline();
}

VkPipelineShaderStageCreateInfo CreatePipelineShaderInfo(VkShaderStageFlagBits stage, VkShaderModule shader)
{
    return VkPipelineShaderStageCreateInfo
    {
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        nullptr,
        0,
        stage,
        shader,
        "main",
        nullptr
    };
}


void Pipeline::CreateCameraDescriptorSetLayout()
{
    DescriptorSetMaker cameraSetMaker;
    cameraSetMaker.CreateLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    cameraSetMaker.CreateDescriptorSetLayout();
    cameraSetMaker.CreateDescriptorPool(3);

    descriptorSetLayouts.push_back(cameraSetMaker.layout);
    setMakers.push_back(cameraSetMaker);
}

void Pipeline::CreateObjectDescriptorSetLayout()
{
    DescriptorSetMaker objectSetMaker;
    objectSetMaker.CreateLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    objectSetMaker.CreateDescriptorSetLayout();
    objectSetMaker.CreateDescriptorPool(1000);

    descriptorSetLayouts.push_back(objectSetMaker.layout);
    setMakers.push_back(objectSetMaker);
}

void Pipeline::CreatePipeline()
{
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    scissor.offset = {0, 0};
    scissor.extent = Engine::engine->extent;

    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)Engine::engine->extent.width;
    viewport.height = (float)Engine::engine->extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;


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
    pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(Engine::engine->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
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

    pipelineInfo.stageCount = shaders.size();
    pipelineInfo.pStages = shaders.data();
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;

    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterization;


    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = SwapChain::swapChain->renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VkResult result = vkCreateGraphicsPipelines(Engine::engine->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
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
        0,
        topology,
        VK_FALSE
    };
}

VkShaderModule CreateShaderModuleFromFile(std::string filename)
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
    CreateInfo.pCode = reinterpret_cast<const unsigned int *>(buffer.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(Engine::engine->device, &CreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
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

std::vector<VkVertexInputAttributeDescription> CreateAttributeDescriptions(std::vector<int> offsets, std::vector<int> sizes, int binding)
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(offsets.size());
    for(size_t i = 0; i < offsets.size(); i++)
    {
        attributeDescriptions[i].binding = binding;
        attributeDescriptions[i].location = i;
        attributeDescriptions[i].format = (VkFormat)(100 + 3*(sizes[i]/sizeof(float) - 1));
        attributeDescriptions[i].offset = offsets[i];
    }

    return attributeDescriptions;
}
