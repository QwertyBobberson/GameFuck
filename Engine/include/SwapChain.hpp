#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include "Engine.hpp"

struct SwapChainSupportDetails;

struct SwapChain
{
    Engine* engine;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> frameBuffers;
    VkImageView depthImageView;
    VkFormat format;
    VkExtent2D extent;
};

void CreateImageViews(SwapChain &swapChain);

VkExtent2D ChooseSwapExtent(Engine* engine, const VkSurfaceCapabilitiesKHR &capabilities);

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

SwapChain CreateSwapChain(Engine* engine, CommandPool cmdPool);

void CreateFramebuffers(SwapChain &swapChain, VkRenderPass renderPass);

#include "Helpers.hpp"