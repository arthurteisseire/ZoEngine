#include <string>
#include "Window.h"
#include "Logger.h"


bool Window::init(unsigned int width, unsigned int height, std::string title)
{
    if (!glfwInit())
    {
        Logger::error("%s: glfwInit() error\n", __FUNCTION__);
        return false;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        Logger::error("%s: could not create window\n", __FUNCTION__);
        glfwTerminate();
        return false;
    }

    Logger::info("%s: Window successfully initialized\n", __FUNCTION__);
    return true;
}

void Window::mainLoop()
{
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
    }
}

void Window::cleanup()
{
    Logger::info("%s: Terminating window\n", __FUNCTION__);
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
