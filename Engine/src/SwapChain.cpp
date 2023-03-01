#include "../include/SwapChain.hpp"

SwapChain* SwapChain::swapChain = nullptr;

SwapChain::SwapChain() : currentFrame(0)
{
    if(swapChain == nullptr)
    {
        swapChain = this;
    }

    Create();

    swapChainImages = Image::CreateImagesFromSwapchain(swapchain, Engine::engine->surfaceFormat.format, Engine::engine->extent);
    depthImage = Image::CreateDepthImage(Engine::engine->extent);
    CreateSyncObjects();
    CreateRenderPass();
    CreateFramebuffers();
}

void SwapChain::Create()
{
    unsigned int imageCount = Engine::engine->swapChainSupport.capabilities.minImageCount + 1;
    if (Engine::engine->swapChainSupport.capabilities.maxImageCount > 0 && imageCount > Engine::engine->swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = Engine::engine->swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    CreateInfo.surface = Engine::engine->surface;

    CreateInfo.minImageCount = imageCount;
    CreateInfo.imageFormat = Engine::engine->surfaceFormat.format;
    CreateInfo.imageColorSpace = Engine::engine->surfaceFormat.colorSpace;
    CreateInfo.imageExtent = Engine::engine->extent;
    CreateInfo.imageArrayLayers = 1;
    CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = Device::FindQueueFamilies(Device::physicalDevice, Engine::engine->surface);
    unsigned int queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily)
    {
        CreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        CreateInfo.queueFamilyIndexCount = 2;
        CreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    CreateInfo.preTransform = Engine::engine->swapChainSupport.capabilities.currentTransform;
    CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    CreateInfo.presentMode = Engine::engine->presentMode;
    CreateInfo.clipped = VK_TRUE;

    CreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(Engine::engine->device, &CreateInfo, nullptr, &swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to Create swap chain!");
    }
}

void SwapChain::CreateFramebuffers()
{
    for(unsigned int i = 0; i < swapChainImages.size(); i++)
    {
        Image attachments[] {swapChainImages[i], depthImage};
        FrameBuffer framebuffer(attachments, 2, renderPass, Engine::engine->extent);
        frameBuffers.push_back(framebuffer);
    }
}

void SwapChain::Destroy()
{
    depthImage.Destroy();
    for(size_t i = 0; i < Engine::engine->MaxFramesInFlight; i++)
    {
        swapChainImages[i].Destroy();
        vkDestroyFramebuffer(Engine::engine->device, frameBuffers[i].framebuffer, nullptr);
    }

    vkDestroySwapchainKHR(Engine::engine->device, swapchain, nullptr);
}

void SwapChain::CreateSyncObjects()
{
    imageAvailableSemaphores.resize(Engine::engine->MaxFramesInFlight);
    renderFinishedSemaphores.resize(Engine::engine->MaxFramesInFlight);
    inFlightFences.resize(Engine::engine->MaxFramesInFlight);


    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(size_t i = 0; i < Engine::engine->MaxFramesInFlight; i++)
    {
        if(vkCreateSemaphore(Engine::engine->device, &semaphoreInfo, nullptr, &(imageAvailableSemaphores[i])) != VK_SUCCESS ||
            vkCreateSemaphore(Engine::engine->device, &semaphoreInfo, nullptr, &(renderFinishedSemaphores[i])) != VK_SUCCESS ||
            vkCreateFence(Engine::engine->device, &fenceInfo, nullptr, &(inFlightFences[i])) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects for a frame");
        }
    }
}

void SwapChain::CreateRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = Engine::engine->surfaceFormat.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment;
    depthAttachment.flags = 0;
    depthAttachment.format = Device::FindDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<unsigned int>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(Engine::engine->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to Create render pass!");
    }
}

void SwapChain::RecreateSwapchain()
{
    SwapChain::swapChain->Destroy();
    SwapChain::swapChain->Create();
    SwapChain::swapChain->swapChainImages = Image::CreateImagesFromSwapchain(SwapChain::swapChain->swapchain, Engine::engine->surfaceFormat.format, Engine::engine->extent);
    SwapChain::swapChain->depthImage = Image::CreateDepthImage(Engine::engine->extent);
}