#ifndef ZOENGINE_VKRENDERER_H
#define ZOENGINE_VKRENDERER_H


#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include "VkRenderData.h"

class VkRenderer
{
public:
    explicit VkRenderer(GLFWwindow *window);
    bool init(unsigned int width, unsigned int height);
    void setSize(unsigned int width, unsigned int height);
    bool uploadData(BasicMesh vertexData);
    bool draw();
    void cleanup();

private:
    VkRenderData mRenderData{};

    int mTriangleCount = 0;

    GLFWwindow *mWindow = nullptr;

    VkSurfaceKHR mSurface = VK_NULL_HANDLE;
    vkb::PhysicalDevice mPhysDevice;

    VkBuffer mVertexBuffer;
    VmaAllocation mVertexBufferAlloc;

    bool deviceInit();
    bool getQueue();
    bool createDepthBuffer();
    bool createSwapchain();
    bool createRenderPass();
    bool createPipeline();
    bool createFramebuffer();
    bool createCommandPool();
    bool createCommandBuffer();
    bool createSyncObjects();
    bool loadTexture();

    bool initVma();

    bool recreateSwapchain();
};


#endif //ZOENGINE_VKRENDERER_H
