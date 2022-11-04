#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include "../include/SwapChain.hpp"

void CreateImageViews(SwapChain &swapChain)
{
    swapChain.swapChainImageViews.resize(swapChain.swapChainImages.size());

    for (size_t i = 0; i < swapChain.swapChainImages.size(); i++)
    {
        swapChain.swapChainImageViews[i] = CreateImageView(swapChain.engine, swapChain.swapChainImages[i], swapChain.format, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

VkExtent2D ChooseSwapExtent(Engine* engine, const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(engine->window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)};

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

SwapChain CreateSwapChain(Engine* engine, CommandPool cmdPool)
{
    SwapChain swapChain;
    swapChain.engine = engine;
    swapChain.extent = {engine->width, engine->height};
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(engine->physicalDevice, engine->surface);

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(engine, swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    CreateInfo.surface = engine->surface;

    CreateInfo.minImageCount = imageCount;
    CreateInfo.imageFormat = surfaceFormat.format;
    CreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    CreateInfo.imageExtent = extent;
    CreateInfo.imageArrayLayers = 1;
    CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = FindQueueFamilies(engine->physicalDevice, engine->surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

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

    CreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    CreateInfo.presentMode = presentMode;
    CreateInfo.clipped = VK_TRUE;

    CreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(engine->device, &CreateInfo, nullptr, &swapChain.swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to Create swap chain!");
    }

    vkGetSwapchainImagesKHR(engine->device, swapChain.swapChain, &imageCount, nullptr);
    swapChain.swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(engine->device, swapChain.swapChain, &imageCount, swapChain.swapChainImages.data());

    swapChain.format = surfaceFormat.format;
    swapChain.extent = extent;

    CreateImageViews(swapChain);
    swapChain.depthImageView = CreateDepthImage(cmdPool, swapChain).imageView;

    return swapChain;
}


void CreateFramebuffers(SwapChain &swapChain, VkRenderPass renderPass)
{
    swapChain.frameBuffers.resize(swapChain.swapChainImageViews.size());

    for (size_t i = 0; i < swapChain.swapChainImageViews.size(); i++)
    {
        std::array<VkImageView, 2> attachments = {
            swapChain.swapChainImageViews[i],
            swapChain.depthImageView};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChain.extent.width;
        framebufferInfo.height = swapChain.extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(swapChain.engine->device, &framebufferInfo, nullptr, &swapChain.frameBuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to Create framebuffer!");
        }
    }
}