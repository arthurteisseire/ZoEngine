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
    m_applicationName = title;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(
            static_cast<int>(width),
            static_cast<int>(height),
            title.c_str(),
            nullptr,
            nullptr
    );
    if (!m_window)
    {
        Logger::error("%s: could not create window\n", __FUNCTION__);
        glfwTerminate();
        return false;
    }
    Logger::info("%s: Window successfully initialized\n", __FUNCTION__);

    if (!initVulkan())
    {
        Logger::error("%s: Could not init Vulkan\n", __FUNCTION__);
        glfwTerminate();
        return false;
    }

    // Window close event
    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowCloseCallback(m_window, [](GLFWwindow *window) {
        auto thisWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
        thisWindow->handleWindowCloseEvents();
    });

    // Keyboard events
    glfwSetKeyCallback(m_window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto thisWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
        thisWindow->handleKeyEvents(key, scancode, action, mods);
    });

    // Mouse events
    glfwSetMouseButtonCallback(m_window, [](GLFWwindow *window, int button, int action, int mods) {
        auto thisWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
        thisWindow->handleMouseButtonEvents(button, action, mods);
    });

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
    m_instance.destroySurfaceKHR(m_surface, nullptr);
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::initVulkan()
{
    uint32_t extensionCount = 0;
    const char **extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

    if (extensionCount == 0)
    {
        Logger::error("%s error: no Vulkan extensions found, need at least 'VK_KHR_surface'\n", __FUNCTION__);
        return false;
    }

    Logger::info("%s: Found %u Vulkan extensions\n", __FUNCTION__, extensionCount);
    for (int i = 0; i < extensionCount; ++i)
    {
        Logger::error("%s: %s\n", __FUNCTION__, std::string(extensions[i]).c_str());
    }

    vk::ApplicationInfo appInfo(
            m_applicationName.c_str(),
            VK_MAKE_VERSION(0, 0, 1),
            "Game Animations Programming",
            VK_MAKE_VERSION(0, 1, 0),
            VK_MAKE_VERSION(0, 1, 1)
    );
    m_instance = vk::createInstance(
            vk::InstanceCreateInfo(
                    vk::InstanceCreateFlags(),
                    &appInfo,
                    0,
                    nullptr, // No layers
                    extensionCount,
                    extensions
            )
    );

    uint32_t physicalDeviceCount = 0;
    m_instance.enumeratePhysicalDevices(&physicalDeviceCount, nullptr);
    if (physicalDeviceCount == 0)
    {
        Logger::info("%s: No Vulkan capable GPU found\n", __FUNCTION__);
        return false;
    }
    std::vector<vk::PhysicalDevice> devices(physicalDeviceCount);
    {
        vk::Result result = m_instance.enumeratePhysicalDevices(&physicalDeviceCount, devices.data());
        if (result != vk::Result::eSuccess)
        {
            Logger::error("%s: Failed to enumerate physical devices", __FUNCTION__);
            return false;
        }
        Logger::info("%s: Found %u physical device(s)\n", __FUNCTION__, physicalDeviceCount);
    }

    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(m_instance, m_window, nullptr, &surface);
    if (result != VK_SUCCESS)
    {
        Logger::error("%s: Could not create Vulkan surface\n", __FUNCTION__);
        return false;
    }
    m_surface = vk::SurfaceKHR(surface);

    return true;
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
