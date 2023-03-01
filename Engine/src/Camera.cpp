#include "../include/Camera.hpp"


Camera::Camera(Pipeline pipeline, float fov, float nearPlane, float farPlane, float aspectRatio) : pipeline(pipeline)
{
    transform = Transform
    {
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::mat4(1.0f),
        glm::mat4(1.0f)
    };

    viewProjection = ViewProjection
    {
        glm::inverse(transform.pos),
        glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane)
    };

    for(unsigned int i = 0; i < Engine::engine->MaxFramesInFlight; i++)
    {
        Buffer ubo(sizeof(Transform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        ubo.CopyFromCPU(&viewProjection, sizeof(ViewProjection));

        descriptorSets.push_back(DescriptorSet({Descriptor(ubo)}));

        descriptorSets[i].set = (pipeline.setMakers[0].CreateDescriptorSet(descriptorSets[i].descriptors));
    }
}

void Camera::BeginDraw()
{
    vkWaitForFences(Engine::engine->device, 1, &(SwapChain::swapChain->inFlightFences[SwapChain::swapChain->currentFrame]), VK_TRUE, UINT64_MAX);
	vkAcquireNextImageKHR(Engine::engine->device, SwapChain::swapChain->swapchain, UINT64_MAX, SwapChain::swapChain->imageAvailableSemaphores[SwapChain::swapChain->currentFrame], NULL, &imageIndex);
    viewProjection.view = glm::inverse(transform.pos * transform.rotation);

    descriptorSets[SwapChain::swapChain->currentFrame].descriptors[0].data.buffer.CopyFromCPU((void*)&viewProjection, sizeof(ViewProjection));

    vkResetFences(Engine::engine->device, 1, &(SwapChain::swapChain->inFlightFences[SwapChain::swapChain->currentFrame]));
    vkResetCommandBuffer(Engine::engine->cmdPool->commandBuffers[SwapChain::swapChain->currentFrame], 0);


	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(Engine::engine->cmdPool->commandBuffers[SwapChain::swapChain->currentFrame], &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin Recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = SwapChain::swapChain->renderPass;
	renderPassInfo.framebuffer = SwapChain::swapChain->frameBuffers[imageIndex].framebuffer;
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = Engine::engine->extent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<unsigned int>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();


	vkCmdBeginRenderPass(Engine::engine->cmdPool->commandBuffers[SwapChain::swapChain->currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindDescriptorSets(Engine::engine->cmdPool->commandBuffers[SwapChain::swapChain->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &descriptorSets[imageIndex].set, 0, nullptr);
}

void Camera::DrawObject(RenderObject obj)
{
	VkDeviceSize offsets[1] = {0};
    obj.descriptorSets[SwapChain::swapChain->currentFrame].descriptors[0].data.buffer.CopyFromCPU((void*)&obj.transform, sizeof(Transform));
	vkCmdBindPipeline(Engine::engine->cmdPool->commandBuffers[SwapChain::swapChain->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
	vkCmdBindVertexBuffers(Engine::engine->cmdPool->commandBuffers[SwapChain::swapChain->currentFrame], 0, 1, &obj.mesh.vertexBuffer.buffer, {offsets});
	vkCmdBindIndexBuffer(Engine::engine->cmdPool->commandBuffers[SwapChain::swapChain->currentFrame], obj.mesh.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(Engine::engine->cmdPool->commandBuffers[SwapChain::swapChain->currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 1, 1, &obj.descriptorSets[SwapChain::swapChain->currentFrame].set, 0, nullptr);
	vkCmdDrawIndexed(Engine::engine->cmdPool->commandBuffers[SwapChain::swapChain->currentFrame], static_cast<unsigned int>(obj.mesh.indexCount), 1, 0, 0, 0);
}

void Camera::EndDraw()
{
	vkCmdEndRenderPass(Engine::engine->cmdPool->commandBuffers[SwapChain::swapChain->currentFrame]);
	if (vkEndCommandBuffer(Engine::engine->cmdPool->commandBuffers[SwapChain::swapChain->currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to Record command buffer!");
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {SwapChain::swapChain->imageAvailableSemaphores[SwapChain::swapChain->currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &(Engine::engine->cmdPool->commandBuffers[SwapChain::swapChain->currentFrame]);


    VkSemaphore signalSemaphores[] = {SwapChain::swapChain->renderFinishedSemaphores[SwapChain::swapChain->currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

	if(vkQueueSubmit(Engine::engine->graphicsQueue, 1, &submitInfo, SwapChain::swapChain->inFlightFences[SwapChain::swapChain->currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit command buffer");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {SwapChain::swapChain->swapchain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(Engine::engine->presentQueue, &presentInfo);
    SwapChain::swapChain->currentFrame = (SwapChain::swapChain->currentFrame + 1) % Engine::engine->MaxFramesInFlight;
}