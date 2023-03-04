#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "../include/Descriptor.hpp"

Data::Data(){};

Descriptor::Descriptor(Buffer buffer)
{
    data.buffer = buffer;

    VkDescriptorBufferInfo* bufferInfo = new VkDescriptorBufferInfo();
    bufferInfo->buffer = buffer.buffer;
    bufferInfo->offset = 0;
    bufferInfo->range = buffer.size;

    info = bufferInfo;
    type = DescriptorType::buffer;
}

Descriptor::Descriptor(Image image)
{
    data.image = image;

    VkDescriptorImageInfo* imageInfo = new VkDescriptorImageInfo();
    imageInfo->imageLayout = image.layout;
    imageInfo->imageView = image.view;
    imageInfo->sampler = image.CreateSampler();

    info = imageInfo;
    type = DescriptorType::image;
}