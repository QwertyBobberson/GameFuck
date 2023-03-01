#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Image.hpp"

class FrameBuffer
{
public:
    VkFramebuffer framebuffer;

    VkExtent2D extent;
    FrameBuffer(Image* atachments, unsigned int attachmentCount, VkRenderPass renderPass, VkExtent2D extent);
};
