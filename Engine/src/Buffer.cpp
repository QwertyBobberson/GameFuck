
#include "../include/Buffer.hpp"

Buffer::Buffer() {}

Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) : size(size)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(Engine::engine->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vertex buffers");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(Engine::engine->device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Device::FindMemoryType(memRequirements.memoryTypeBits, properties);

    if(vkAllocateMemory(Engine::engine->device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate vertex buffer memory");
    }

    vkBindBufferMemory(Engine::engine->device, buffer, memory, 0);
}

void Buffer::CopyToImage(Image dst)
{
    VkCommandBuffer commandBuffer = Engine::engine->cmdPool->BeginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {dst.extent.width, dst.extent.height, 1};

    vkCmdCopyBufferToImage(commandBuffer, buffer, dst.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    Engine::engine->cmdPool->EndSingleTimeCommands(commandBuffer);
}

void Buffer::CopyFromCPU(void* data, size_t dataSize)
{
    void* dst;
    vkMapMemory(Engine::engine->device, memory, 0, dataSize, 0, &dst);
    memcpy(dst, data, size);
    vkUnmapMemory(Engine::engine->device, memory);
}

void Buffer::CopyToBuffer(Buffer dst)
{
    VkCommandBuffer commandBuffer = Engine::engine->cmdPool->BeginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, buffer, dst.buffer, 1, &copyRegion);

    Engine::engine->cmdPool->EndSingleTimeCommands(commandBuffer);
}

Buffer Buffer::CreateVertexBuffer(void* vertexData, VkDeviceSize bufferSize)
{
    Buffer stagingBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.CopyFromCPU(vertexData, bufferSize);

    Buffer buffer = Buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    stagingBuffer.CopyToBuffer(buffer);

    stagingBuffer.Destroy();
    return buffer;
}

Buffer Buffer::CreateIndexBuffer(void* indexData, VkDeviceSize bufferSize)
{
    Buffer stagingBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.CopyFromCPU(indexData, bufferSize);

    Buffer buffer = Buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    stagingBuffer.CopyToBuffer(buffer);

    stagingBuffer.Destroy();
    return buffer;
}

size_t Buffer::CopyToCPU(void* dst)
{
    void* buffer;
    vkMapMemory(Engine::engine->device, memory, 0, size, 0, &buffer);
    memcpy(dst, buffer, size);
    vkUnmapMemory(Engine::engine->device, memory);
    return size;
}


void Buffer::Destroy()
{
    vkDestroyBuffer(Engine::engine->device, buffer, nullptr);
    vkFreeMemory(Engine::engine->device, memory, nullptr);
}
