#include "../include/SwapChain.hpp"
#include "../include/Engine.hpp"
#include "../include/RenderPass.hpp"
#include "../include/Device.hpp"

#include <iostream>
#include <stdexcept>

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
    new RenderPass(true);
    new RenderPass(false);
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
        FrameBuffer framebuffer(attachments, 2, Engine::engine->extent);
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

void SwapChain::RecreateSwapchain()
{
    SwapChain::swapChain->Destroy();
    SwapChain::swapChain->Create();
    SwapChain::swapChain->swapChainImages = Image::CreateImagesFromSwapchain(SwapChain::swapChain->swapchain, Engine::engine->surfaceFormat.format, Engine::engine->extent);
    SwapChain::swapChain->depthImage = Image::CreateDepthImage(Engine::engine->extent);
}