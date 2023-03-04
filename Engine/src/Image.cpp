#include "../include/Image.hpp"
#include "../include/Engine.hpp"
#include "../include/Device.hpp"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <memory.h>

Image::Image(VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,  VkImageAspectFlags aspectFlags, VkExtent2D extent) : format(format), layout (VK_IMAGE_LAYOUT_UNDEFINED), extent(extent)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = extent.width;
    imageInfo.extent.height = extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;

    imageInfo.format = format;
    imageInfo.tiling = tiling;

    imageInfo.initialLayout = layout;
    imageInfo.usage = usage;

    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0;

    if(vkCreateImage(Engine::engine->device, &imageInfo, nullptr, &image) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(Engine::engine->device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};

    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Device::FindMemoryType(memRequirements.memoryTypeBits, properties);

    if(vkAllocateMemory(Engine::engine->device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(Engine::engine->device, image, memory, 0);

    if(usage & (VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_STORAGE_BIT|VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT|VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT|VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT|VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT|VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR|VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT|VK_IMAGE_USAGE_SAMPLE_WEIGHT_BIT_QCOM|VK_IMAGE_USAGE_SAMPLE_BLOCK_MATCH_BIT_QCOM))
    {
        view = CreateView(image, format, aspectFlags);
    }
}

Image::Image(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkExtent2D extent) : image(image), view(CreateView(image, format, aspectFlags)), format(format), layout(VK_IMAGE_LAYOUT_UNDEFINED), extent(extent) {}

std::vector<Image> Image::CreateImagesFromSwapchain(VkSwapchainKHR swapchain, VkFormat format, VkExtent2D extent)
{
    std::vector<VkImage> swapChainImages;
    unsigned int imageCount;
    vkGetSwapchainImagesKHR(Engine::engine->device, swapchain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(Engine::engine->device, swapchain, &imageCount, swapChainImages.data());
    std::vector<Image> images;
    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        images.push_back(Image(swapChainImages[i], format, VK_IMAGE_ASPECT_COLOR_BIT, extent));
    }

    return images;
}

Image Image::CreateDepthImage(VkExtent2D extent)
{
    VkFormat depthFormat = Device::FindDepthFormat();
    Image image = Image(depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, extent);
    image.TransitionLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    return image;
}

void Image::TransitionLayout(VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = Engine::engine->cmdPool->BeginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = layout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = 0;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if(layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if(layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if(layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    else if(layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    else if(layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if(layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if(layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if(layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = 0;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    else if(layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if(layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    else if(layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else
    {
        barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }

    if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        if(Device::hasStencilComponent(format))
        {
            barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        }
    }
    else
    {
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    Engine::engine->cmdPool->EndSingleTimeCommands(commandBuffer);
    layout = newLayout;
}

VkSampler Image::CreateSampler()
{
    VkSampler sampler;
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;

    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    samplerInfo.anisotropyEnable = VK_TRUE;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(Device::physicalDevice, &properties);

    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if(vkCreateSampler(Engine::engine->device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler");
    }

    return sampler;
}

void Image::Destroy()
{
    vkDestroyImageView(Engine::engine->device, view, nullptr);
    vkDestroyImage(Engine::engine->device, image, nullptr);
    vkFreeMemory(Engine::engine->device, memory, nullptr);
}

void Image::CopyToImage(Image dst)
{
    VkImageLayout srcLayout = layout;
    VkImageLayout dstLayout = dst.layout;

    dst.TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    VkCommandBuffer cmdBuffer = Engine::engine->cmdPool->BeginSingleTimeCommands();
    VkImageCopy copy;
    copy.srcOffset = {0, 0, 0};
    copy.dstOffset = {0, 0, 0};

    copy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.srcSubresource.mipLevel = 0;
    copy.srcSubresource.baseArrayLayer = 0;
    copy.srcSubresource.layerCount = 1;

    copy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.dstSubresource.mipLevel = 0;
    copy.dstSubresource.baseArrayLayer = 0;
    copy.dstSubresource.layerCount = 1;

    copy.extent = {Engine::engine->extent.width, Engine::engine->extent.height, 1};
    vkCmdCopyImage(cmdBuffer, image, layout, dst.image, dst.layout, 1, &copy);
    Engine::engine->cmdPool->EndSingleTimeCommands(cmdBuffer);

    dst.TransitionLayout(dstLayout != VK_IMAGE_LAYOUT_UNDEFINED ? dstLayout : VK_IMAGE_LAYOUT_GENERAL);
    TransitionLayout(srcLayout);
}

void Image::CopyToCPU(char* dst)
{
    TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    Image stagingImg(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_IMAGE_ASPECT_COLOR_BIT, extent);
    CopyToImage(stagingImg);
    void* src;
    vkMapMemory(Engine::engine->device, stagingImg.memory, 0, extent.height * extent.width * 4, 0, &src);
    memcpy(dst, src, extent.height * extent.width * 4);
    vkUnmapMemory(Engine::engine->device, stagingImg.memory);
}

void Image::SaveToBMP(char* fileName)
{
    printf("Starting save to bmp\n");
    char* pixelData = (char*)malloc(extent.height * extent.width * 4);
    CopyToCPU(pixelData);
    printf("Got pixel data, %ux%u pixels (%u bytes)\n", extent.width, extent.height, extent.height * extent.width * 4);

    FILE* file;
    file = fopen(fileName, "wb");
    // char* bmpData = (char*)malloc(54 + extent.width * extent.height * 4);
    //Filetype
    const char* fileType = "BM";
    fwrite(fileType, 2, 1, file);
    //Size of file
    unsigned int fileSize = 54 + extent.width * extent.height * 4;
    fwrite(&fileSize, 4, 1, file);
    //Depends on program writing file
    unsigned int programStuff = 0;
    fwrite(&programStuff, 4, 1, file);
    //Offset from start of file to start of pixels
    unsigned int startOfData = 54;
    fwrite(&startOfData, 4, 1, file);
    //Size of header (determined by header type)
    unsigned int headerSize = 40;
    fwrite(&headerSize, 4, 1, file);
    //Width and Length in pixels
    unsigned int width = extent.width;
    unsigned int height = extent.height;
    fwrite(&width, 4, 1, file);
    fwrite(&height, 4, 1, file);
    //Number of planes and bits per pixel
    unsigned short planes = 1;
    unsigned short pixelSize = 32;
    fwrite(&planes, 2, 1, file);
    fwrite(&pixelSize, 2, 1, file);
    //Compression style and size of uncompressed data
    unsigned int compressionStyle = 0;
    unsigned int uncompressedSize = 0;
    fwrite(&compressionStyle, 4, 1, file);
    fwrite(&uncompressedSize, 4, 1, file);
    //Pixel Density (Pixels/Meter)
    unsigned int pixelDensityX = 3779;
    unsigned int pixelDensityY = 3779;
    fwrite(&pixelDensityX, 4, 1, file);
    fwrite(&pixelDensityY, 4, 1, file);
    //Number of unique and important colors
    unsigned int uniqueColors = 0;
    unsigned int importantColors = 0;
    fwrite(&uniqueColors, 4, 1, file);
    fwrite(&importantColors, 4, 1, file);

    fwrite(pixelData, extent.width * extent.height * 4, 1, file);
}

VkImageView Image::CreateView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageView view;

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if(vkCreateImageView(Engine::engine->device, &viewInfo, nullptr, &view) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture image view!");
    }

    return view;
}