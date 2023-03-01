#include "../include/Input.hpp"

Keyboard Input::keyboardState;
MouseButtons Input::mouseButtonsState;
glm::vec2 Input::mousePos;
glm::vec2 Input::mouseScroll;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Input::keyboardState[key] = action != GLFW_RELEASE;
}

void MousePosCallback(GLFWwindow* window, double xPos, double yPos)
{
    Input::mousePos = glm::vec2(xPos, yPos);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Input::mouseButtonsState[button] = action != GLFW_RELEASE;
}

void MouseScrollCallback(GLFWwindow* window, double x, double y)
{
    Input::mouseScroll = glm::vec2(x, y);
}