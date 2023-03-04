#include "../include/FrameBuffer.hpp"
#include "../include/RenderPass.hpp"
#include "../include/Engine.hpp"

#include <stdexcept>

FrameBuffer::FrameBuffer(Image* images, unsigned int attachmentCount, VkExtent2D extent) : extent(extent), image(images[0])
{
    VkImageView* attachments = new VkImageView[attachmentCount];

    for(unsigned int i = 0; i < attachmentCount; i++)
    {
        attachments[i] = images[i].view;
    }
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = RenderPass::swapChainRenderPass->renderpass;
    framebufferInfo.attachmentCount = static_cast<unsigned int>(attachmentCount);
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(Engine::engine->device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to Create framebuffer!");
    }
}

FrameBuffer::FrameBuffer(Image image) : extent(image.extent), image(image)
{
    Image depthImage = Image::CreateDepthImage(image.extent);
    VkImageView* attachments = new VkImageView[2];

    attachments[0] = image.view;
    attachments[1] = depthImage.view;

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = RenderPass::swapChainRenderPass->renderpass;
    framebufferInfo.attachmentCount = static_cast<unsigned int>(2);
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(Engine::engine->device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to Create framebuffer!");
    }
}