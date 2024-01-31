#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#define GGLFW_INCLUDE_VULKAN

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <VkBootstrap.h>
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
