#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class RenderPass
{
    public:
        static RenderPass* swapChainRenderPass;
        static RenderPass* headlessRenderPass;
        VkRenderPass renderpass;

        RenderPass(bool swapChainTarget);
};
