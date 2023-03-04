#pragma once

#include "HelperStructs.hpp"
#include "DescriptorSet.hpp"
#include "Pipeline.hpp"
#include "RenderObject.hpp"
#include "FrameBuffer.hpp"
#include "Engine.hpp"

#include <vector>

class Camera
{
    unsigned int imageIndex;
    unsigned int numTargets;
    unsigned int currentFrame;
    bool drawToSwapchain;
public:
    Pipeline pipeline;
    Transform transform;
    ViewProjection viewProjection;
    std::vector<FrameBuffer> targets;
    std::vector<DescriptorSet> descriptorSets;
    Camera(Pipeline pipeline, float fov = 60.0f, float nearPlane = 0.1f, float farPlane = 1000.0f, float aspectRatio = Engine::engine->extent.width/(float)Engine::engine->extent.height);

    void SetTarget(FrameBuffer target);
    void SetTarget(std::vector<FrameBuffer> targets);
    void UnsetTarget();



    void BeginDraw();
    void DrawObject(RenderObject obj);
    void EndDraw();
};
