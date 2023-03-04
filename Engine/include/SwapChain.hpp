#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

#include "FrameBuffer.hpp"

struct SwapChain
{
    static SwapChain* swapChain;
    VkSwapchainKHR swapchain;

    std::vector<Image> swapChainImages;
    std::vector<FrameBuffer> frameBuffers;
    Image depthImage;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

    int currentFrame;

    SwapChain();
    void CreateImages(unsigned int image);
    void CreateSyncObjects();
    void Create();
    void CreateFramebuffers();

    void RecreateSwapchain();

    void Destroy();

    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
};