#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../include/Camera.hpp"
#include "../include/SwapChain.hpp"
#include "../include/RenderPass.hpp"
#include "../include/Engine.hpp"

#include <array>
#include <stdexcept>

Camera::Camera(Pipeline pipeline, float fov, float nearPlane, float farPlane, float aspectRatio) : pipeline(pipeline), drawToSwapchain(true), numTargets(Engine::engine->MaxFramesInFlight), targets(SwapChain::swapChain->frameBuffers), currentFrame(0)
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


void Camera::SetTarget(FrameBuffer target)
{
	SetTarget(std::vector<FrameBuffer>{target});
}
void Camera::SetTarget(std::vector<FrameBuffer> targets)
{
	drawToSwapchain = false;
	this->targets = targets;
}
void Camera::UnsetTarget()
{
	drawToSwapchain = true;
	targets.clear();
	targets = SwapChain::swapChain->frameBuffers;
	numTargets = Engine::engine->MaxFramesInFlight;
}

void Camera::BeginDraw()
{
	if(drawToSwapchain)
	{
		vkWaitForFences(Engine::engine->device, 1, &(SwapChain::swapChain->inFlightFences[currentFrame]), VK_TRUE, UINT64_MAX);
		vkAcquireNextImageKHR(Engine::engine->device, SwapChain::swapChain->swapchain, UINT64_MAX, SwapChain::swapChain->imageAvailableSemaphores[currentFrame], NULL, &imageIndex);
	}
	vkResetFences(Engine::engine->device, 1, &(SwapChain::swapChain->inFlightFences[currentFrame]));
	vkResetCommandBuffer(Engine::engine->cmdPool->commandBuffers[currentFrame], 0);

	viewProjection.view = glm::inverse(transform.pos * transform.rotation);
    descriptorSets[currentFrame].descriptors[0].data.buffer.CopyFromCPU((void*)&viewProjection, sizeof(ViewProjection));

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(Engine::engine->cmdPool->commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin Recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = drawToSwapchain ? RenderPass::swapChainRenderPass->renderpass : RenderPass::headlessRenderPass->renderpass;
	renderPassInfo.framebuffer = targets[drawToSwapchain ? imageIndex : currentFrame].framebuffer;
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = Engine::engine->extent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<unsigned int>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();


	vkCmdBeginRenderPass(Engine::engine->cmdPool->commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindDescriptorSets(Engine::engine->cmdPool->commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 0, 1, &descriptorSets[currentFrame].set, 0, nullptr);
}

void Camera::DrawObject(RenderObject obj)
{
	VkDeviceSize offsets[1] = {0};
    obj.descriptorSets[currentFrame].descriptors[0].data.buffer.CopyFromCPU((void*)&obj.transform, sizeof(Transform));
	vkCmdBindPipeline(Engine::engine->cmdPool->commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
	vkCmdBindVertexBuffers(Engine::engine->cmdPool->commandBuffers[currentFrame], 0, 1, &obj.mesh.vertexBuffer.buffer, {offsets});
	vkCmdBindIndexBuffer(Engine::engine->cmdPool->commandBuffers[currentFrame], obj.mesh.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(Engine::engine->cmdPool->commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipelineLayout, 1, 1, &obj.descriptorSets[currentFrame].set, 0, nullptr);
	vkCmdDrawIndexed(Engine::engine->cmdPool->commandBuffers[currentFrame], static_cast<unsigned int>(obj.mesh.indexCount), 1, 0, 0, 0);
}

void Camera::EndDraw()
{
	vkCmdEndRenderPass(Engine::engine->cmdPool->commandBuffers[currentFrame]);
	if (vkEndCommandBuffer(Engine::engine->cmdPool->commandBuffers[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to Record command buffer!");
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {SwapChain::swapChain->imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    VkSemaphore signalSemaphores[] = {SwapChain::swapChain->renderFinishedSemaphores[currentFrame]};

	if(drawToSwapchain)
	{
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;
	}
	else
	{
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;
	}

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &(Engine::engine->cmdPool->commandBuffers[currentFrame]);

	if(vkQueueSubmit(Engine::engine->graphicsQueue, 1, &submitInfo, SwapChain::swapChain->inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit command buffer");
	}

	if(drawToSwapchain)
	{
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = {SwapChain::swapChain->swapchain};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &(drawToSwapchain ? imageIndex : currentFrame);

		vkQueuePresentKHR(Engine::engine->presentQueue, &presentInfo);
	}

	currentFrame = (currentFrame + 1) % numTargets;
}