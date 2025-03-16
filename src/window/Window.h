#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include <memory>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <VkBootstrap.h>

#define GGLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include "VkRenderer.h"
#include "model/CustomCube.h"

class Window
{
public:
    bool init(unsigned int width, unsigned int height, const std::string &title);
    void mainLoop();
    void cleanup();

private:
    void handleWindowCloseEvents();
    void handleKeyEvents(int key, int scancode, int action, int mods);
    void handleMouseButtonEvents(int button, int action, int mods);

    GLFWwindow *mWindow = nullptr;
    std::unique_ptr<VkRenderer> mRenderer;
    std::unique_ptr<CustomCube> mModel;
    std::unique_ptr<VMesh> mModelGltf;
    Camera camera;
};

#endif //ENGINE_WINDOW_H
