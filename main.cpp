#include "Engine/src/Engine.cpp"
#include <typeinfo>
using namespace std;


std::vector<Vertex> verts;
std::vector<int> indices;

void DrawSquare(double x, double y, double z, double width, double height, double r, double g, double b)
{
	int start = verts.size();
	verts.push_back({{x			, y			, z}, {r, g, b}, {0.0f, 0.0f, 0.0f}});
	verts.push_back({{x + width	, y			, z}, {r, g, b}, {1.0f, 0.0f, 0.0f}});
	verts.push_back({{x			, y + height, z}, {r, g, b}, {0.0f, 1.0f, 0.0f}});
	verts.push_back({{x + width	, y + height, z}, {r, g, b}, {1.0f, 1.0f, 0.0f}});

	indices.push_back(start + 0);
	indices.push_back(start + 1);
	indices.push_back(start + 2);
	indices.push_back(start + 2);
	indices.push_back(start + 3);
	indices.push_back(start + 1);
}


int main()
{
	DrawSquare(-0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f);

	Engine engine("New Engine Test", 500, 500, 1);
	CommandPool cmdPool(&engine);
	SwapChain swapChain = CreateSwapChain(&engine, cmdPool);
	VkRenderPass renderPass = CreateRenderPass(swapChain);
	Pipeline pipeline;

	VkShaderModule vertShader = CreateShaderModuleFromFile(&engine, "shaders/vert.spv");
	VkShaderModule fragShader = CreateShaderModuleFromFile(&engine, "shaders/frag.spv");

	pipeline.shaders = {CreatePipelineShaderInfo(VK_SHADER_STAGE_VERTEX_BIT, vertShader), CreatePipelineShaderInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragShader)};
	pipeline.inputAssembly = CreateInputCreateInfo();
	CreateFramebuffers(swapChain, renderPass);
	std::vector<VkVertexInputBindingDescription> binding = {CreateBindingDescription(sizeof(Vertex))};
	std::vector<VkVertexInputAttributeDescription> attributes = CreateAttributeDescriptions({offsetof(Vertex, pos), offsetof(Vertex, col), offsetof(Vertex, texCoord)});

	pipeline.vertexInput = CreateVertexInfo({binding}, attributes);

	pipeline.rasterization = CreateRasterizationStateCreateInfo();
	pipeline.renderPass = renderPass;
	CreatePipeline(&engine, pipeline, swapChain);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;


	if (vkBeginCommandBuffer(cmdPool.commandBuffers[0], &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin Recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChain.frameBuffers[0];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = swapChain.extent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clearValues[1].depthStencil = {1.0f, 0};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	VkBuffer vertBuffer;
	VkDeviceMemory vertMem;
	VkBuffer indBuffer;
	VkDeviceMemory indMem;
	VkDeviceSize offsets[1] = {0};
	CreateVertexBuffer(cmdPool, sizeof(Vertex) * verts.size(), verts.data(), vertBuffer, vertMem);
	CreateIndexBuffer(cmdPool, sizeof(int) * indices.size(), indices.data(), indBuffer, indMem);

	vkCmdBeginRenderPass(cmdPool.commandBuffers[0], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(cmdPool.commandBuffers[0], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline);
	vkCmdBindVertexBuffers(cmdPool.commandBuffers[0], 0, 1, &vertBuffer, {offsets});
	vkCmdBindIndexBuffer(cmdPool.commandBuffers[0], indBuffer, 0, VK_INDEX_TYPE_UINT32);

	// Application app;

	// // DrawSquare(-0.5, -0.5, 1, 1, 0, 0, 1);

	// PNG png = ReadPng("test2.png");

	// std::cout << png.height << " " << png.width << std::endl;

	// int sHeight = HEIGHT;
	// int sWidth = WIDTH;

	// std::cout << -1 * sHeight << std::endl;

	// DrawSquare( 0	,  -0.5, 	-0.5,  1, 1, 0, 0, 0);
	// DrawSquare(-0.5	, -0.5,  1.5,  1, 1, 0, 0, 0);

	// try
	// {
	// 	app.Run(verts, indices);
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	return EXIT_FAILURE;
	// }

	// return EXIT_SUCCESS;

}