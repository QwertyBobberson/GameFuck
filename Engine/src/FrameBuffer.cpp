#include "../include/FrameBuffer.hpp"

FrameBuffer::FrameBuffer(Image* images, unsigned int attachmentCount, VkRenderPass renderPass, VkExtent2D extent) : extent(extent)
{
    VkImageView* attachments = new VkImageView[attachmentCount];

    for(unsigned int i = 0; i < attachmentCount; i++)
    {
        attachments[i] = images[i].view;
    }
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
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
