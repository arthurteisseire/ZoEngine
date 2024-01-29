#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include <vulkan/vulkan.hpp>

#define GGLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

class Window
{
public:
    // Glfw
    bool init(unsigned int width, unsigned int height, const std::string &title);
    void mainLoop();
    void cleanup();

    // Vulkan
    bool initVulkan();
private:
    void handleWindowCloseEvents();
    void handleKeyEvents(int key, int scancode, int action, int mods);
    void handleMouseButtonEvents(int button, int action, int mods);

    GLFWwindow *m_window = nullptr;
    std::string m_applicationName;
    vk::Instance m_instance{};
    vk::SurfaceKHR m_surface{};
};

#endif //ENGINE_WINDOW_H
