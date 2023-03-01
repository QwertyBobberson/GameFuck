#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

typedef bool Keyboard[348];
typedef bool MouseButtons[8];

class Input
{
public:
    static Keyboard keyboardState;
    static MouseButtons mouseButtonsState;
    static glm::vec2 mousePos;
    static glm::vec2 mouseScroll;
};
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void MousePosCallback(GLFWwindow* window, double xPos, double yPos);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
