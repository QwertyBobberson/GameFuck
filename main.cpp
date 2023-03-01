
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define STB_IMAGE_IMPLEMENTATION
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_operation.hpp>

#include "Engine/include/Engine.hpp"
#include "Engine/include/SwapChain.hpp"
#include "Engine/include/Camera.hpp"
#include "Engine/include/Input.hpp"
#include <math.h>

Engine engine("New Engine Test", 1000, 500);
SwapChain swapChain;
Pipeline pipeline("shaders/vert.spv", "shaders/frag.spv");
Transform cubeTransform {glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f)};
Transform floorTransform {glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f)), glm::mat4(1.0f), glm::diagonal4x4(glm::vec4(100.0f, 0.0f, 100.0f, 1.0f))};
RenderObject cube = RenderObject::CreateCube(cubeTransform, pipeline);
RenderObject ground = RenderObject::CreateCube(floorTransform, pipeline, 1, 1, 1);
Camera cam(pipeline);

void HandleInput();

int main()
{
	while(!glfwWindowShouldClose(engine.window))
	{
	 	glfwPollEvents();
		HandleInput();
		cam.BeginDraw();
		cam.DrawObject(cube);
		cam.DrawObject(ground);
		cam.EndDraw();
	}

	return 0;
}

glm::vec2 prevMousePos;
glm::vec2 camRotation = glm::vec2(0.0f, 0.0f);

void HandleInput()
{
	if(prevMousePos == glm::vec2(0.0f, 0.0f))
	{
		prevMousePos = Input::mousePos;
	}
	glm::vec2 mouseMovement = Input::mousePos - prevMousePos;

	cam.transform.pos *= glm::rotate(glm::mat4(1.0f), glm::radians(-camRotation.y), glm::vec3(1.0f, 0.0f, 0.0f));

	if(Input::keyboardState[GLFW_KEY_W])
	{
		cam.transform.pos = glm::translate(cam.transform.pos, glm::vec3(0.0f, 0.0f, -0.20f));
	}
	if(Input::keyboardState[GLFW_KEY_S])
	{
		cam.transform.pos = glm::translate(cam.transform.pos, glm::vec3(0.0f, 0.0f, 0.20f));
	}

	if(Input::keyboardState[GLFW_KEY_A])
	{
		cam.transform.pos = glm::translate(cam.transform.pos, glm::vec3(-0.20f, 0.0f, 0.0f));
	}
	if(Input::keyboardState[GLFW_KEY_D])
	{
		cam.transform.pos = glm::translate(cam.transform.pos, glm::vec3(0.20f, 0.0f, 0.0f));
	}

	if(Input::keyboardState[GLFW_KEY_LEFT_SHIFT])
	{
		cam.transform.pos = glm::translate(cam.transform.pos, glm::vec3(0.0f, 0.20f, 0.0f));
	}
	if(Input::keyboardState[GLFW_KEY_SPACE])
	{
		cam.transform.pos = glm::translate(cam.transform.pos, glm::vec3(0.0f, -0.20f, 0.0f));
	}

	if(Input::keyboardState[GLFW_KEY_ESCAPE])
	{
		glfwSetInputMode(engine.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else
	{
		glfwSetInputMode(engine.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	cam.transform.pos *= glm::rotate(glm::mat4(1.0f), glm::radians(camRotation.x), glm::vec3(0.0f, 1.0f, 0.0f));

	camRotation += mouseMovement/5.0f;

	camRotation.x = fmod(camRotation.x, 360.0f);
	camRotation.y = glm::clamp(camRotation.y, -85.0f, 85.0f);

	cam.transform.pos *= glm::rotate(glm::mat4(1.0f), glm::radians(-camRotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
	cam.transform.pos *= glm::rotate(glm::mat4(1.0f), glm::radians(camRotation.y), glm::vec3(1.0f, 0.0f, 0.0f));

	prevMousePos = Input::mousePos;
}