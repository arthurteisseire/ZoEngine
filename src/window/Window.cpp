#include <string>
#include <vector>
#include "Window.h"
#include "Logger.h"


bool Window::init(unsigned int width, unsigned int height, const std::string &title)
{
    if (!glfwInit())
    {
        Logger::error("%s: glfwInit() error\n", __FUNCTION__);
        return false;
    }
    if (!glfwVulkanSupported())
    {
        glfwTerminate();
        Logger::error("%s: Vulkan is not supported\n", __FUNCTION__);
        return false;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    mWindow = glfwCreateWindow(
            static_cast<int>(width),
            static_cast<int>(height),
            title.c_str(),
            nullptr,
            nullptr
    );
    if (!mWindow)
    {
        Logger::error("%s: could not create window\n", __FUNCTION__);
        glfwTerminate();
        return false;
    }
    Logger::info("%s: Window successfully initialized\n", __FUNCTION__);

    mRenderer = std::make_unique<VkRenderer>(mWindow);
    if (!mRenderer->init(width, height))
    {
        glfwTerminate();
        Logger::error("%s: error: Could not init vulkan\n", __FUNCTION__);
        return false;
    }

    mModel = std::make_unique<Model>();
    mModel->init();

    // Window close event
    glfwSetWindowUserPointer(mWindow, this);
    glfwSetWindowCloseCallback(mWindow, [](GLFWwindow *window) {
        auto thisWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
        thisWindow->handleWindowCloseEvents();
    });

    // Keyboard events
    glfwSetKeyCallback(mWindow, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto thisWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
        thisWindow->handleKeyEvents(key, scancode, action, mods);
    });

    // Mouse events
    glfwSetMouseButtonCallback(mWindow, [](GLFWwindow *window, int button, int action, int mods) {
        auto thisWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
        thisWindow->handleMouseButtonEvents(button, action, mods);
    });

    return true;
}

void Window::mainLoop()
{
    mRenderer->uploadData(mModel->getVertexData());

    while (!glfwWindowShouldClose(mWindow))
    {
        if (!mRenderer->draw())
        {
            break;
        }
        glfwPollEvents();
    }
}

void Window::cleanup()
{
    Logger::info("%s: Terminating window\n", __FUNCTION__);
    mRenderer->cleanup();

    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

void Window::handleWindowCloseEvents()
{
    Logger::info("%s: Window closed successfully.\n", __FUNCTION__);
}

void Window::handleKeyEvents(int key, int scancode, int action, int mods)
{
    std::string actionName;
    switch (action)
    {
        case GLFW_PRESS:
            actionName = "pressed";
            break;

        case GLFW_RELEASE:
            actionName = "released";
            break;

        case GLFW_REPEAT:
            actionName = "repeated";
            break;

        default:
            actionName = "invalid";
            break;
    }
    const char *keyName = glfwGetKeyName(key, 0);
    Logger::info("%s: key %s (key %i, scancode %i) %s\n", __FUNCTION__, keyName, key, scancode, actionName.c_str());
}

void Window::handleMouseButtonEvents(int button, int action, int mods)
{
    std::string actionName;
    switch (action)
    {
        case GLFW_PRESS:
            actionName = "pressed";
            break;

        case GLFW_RELEASE:
            actionName = "released";
            break;

        default:
            actionName = "invalid";
            break;
    }

    std::string mouseButtonName;
    switch (button)
    {
        case GLFW_MOUSE_BUTTON_LEFT:
            mouseButtonName = "left";
            break;

        case GLFW_MOUSE_BUTTON_MIDDLE:
            mouseButtonName = "middle";
            break;

        case GLFW_MOUSE_BUTTON_RIGHT:
            mouseButtonName = "right";
            break;

        default:
            mouseButtonName = "not handled yet";
            break;
    }
    Logger::info("%s: %s mouse button %i) %s\n", __FUNCTION__, mouseButtonName.c_str(), button, actionName.c_str());
}
