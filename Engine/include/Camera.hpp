#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "HelperStructs.hpp"
#include "DescriptorSet.hpp"
#include "Pipeline.hpp"
#include "RenderObject.hpp"
#include "SwapChain.hpp"

class Camera
{
    unsigned int imageIndex;
public:
    Pipeline pipeline;
    Transform transform;
    ViewProjection viewProjection;
    std::vector<DescriptorSet> descriptorSets;
    Camera(Pipeline pipeline, float fov = 60.0f, float nearPlane = 0.1f, float farPlane = 1000.0f, float aspectRatio = Engine::engine->extent.width/Engine::engine->extent.height);

    void BeginDraw();
    void DrawObject(RenderObject obj);
    void EndDraw();
};
